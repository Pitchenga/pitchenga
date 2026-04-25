#include "Math.h"
#include "../Util.h"

Math::Math(PitchengaAudioProcessor& proc)
    : Thread("VisualizeWorker"), processor(proc) {
    setupBuffers();
}

Math::~Math() {
    stopThread(2000);
}

void Math::setupBuffers() {
    setupCqtEngine();
    setupCqtBuffers();
    setupPitchDetection();
    setupStft();
}

void Math::setupCqtEngine() {
    Cqt::Config config;
    config.octaves = PitchengaAudioProcessor::numOctaves;
    config.samplingFreq = processor.getSampleRate() > 0 ? processor.getSampleRate() : 44100.0;
    cqtEngine.updateConfig(config);
    cqtEngine.init();

    const int totalBins = cqtEngine.getTotalBins();
    const int binsPerOctave = cqtEngine.getBinsPerOctave();

    pitchClassDetector = std::make_unique<HarmonicPatternPitchClassDetector>(binsPerOctave, config.binsPerSemitone);
    spectralEqualizer = std::make_unique<SpectralEqualizer>(totalBins, 30);
    allBinSmoother = std::make_unique<ExpSmoother>(totalBins, 0.2);
    octaveBinSmoother = std::make_unique<ExpSmoother>(binsPerOctave, 0.05);
}

void Math::setupCqtBuffers() {
    const int totalBins = cqtEngine.getTotalBins();
    const int binsPerOctave = cqtEngine.getBinsPerOctave();
    const auto signalBlockSize = static_cast<size_t>(cqtEngine.getSignalBlockSize());

    workingBuffer.assign(signalBlockSize, 0.0f);
    slidingWindows.assign(PitchengaAudioProcessor::numOctaves, std::vector(signalBlockSize, 0.0f));

    cqtSpectrum.resize(static_cast<size_t>(cqtEngine.getKernelBins()));
    amplitudeSpectrumDb.assign(static_cast<size_t>(totalBins), 0.0);
    octaveBins.assign(static_cast<size_t>(binsPerOctave), 0.0);

    {
        const juce::CriticalSection::ScopedLockType lock(resultLock);
        eyeResults.assign(static_cast<size_t>(binsPerOctave), 0.0);
        rollResults.assign(static_cast<size_t>(totalBins), 0.0);
    }
}

void Math::setupPitchDetection() {
    // --- Pitch Setup ---
    pitchDetector = std::make_unique<sevagh::PitchDetector<float>>(4096);
    rawAudioHistoryBuffer.assign(32768, 0.0f);
    pitchAnalysisBuffer.assign(4096, 0.0f);
}

void Math::setupStft() {
    const double samplingFreq = processor.getSampleRate() > 0 ? processor.getSampleRate() : 44100.0;
    stft.initialize(samplingFreq);
}

void Math::processStft() {
    stft.processFrame(rawAudioHistoryBuffer);
    currentRollPeaks = stft.getPeaks();
}

void Math::updateSampleRate(const double newSampleRate) {
    if (newSampleRate > 0 && std::abs(newSampleRate - cqtEngine.getConfig().samplingFreq) > 0.01) {
        setupBuffers();
    }
}

double Math::amplitudeToDbRescaled(const double amplitude) {
    // Exact port of HarmonRoll's DecibelCalculator.java
    constexpr double zeroAmplitudeDb = -90.30899869919436;
    constexpr double zeroAmplitudeDbInv = 1.0 / zeroAmplitudeDb;

    if (amplitude <= 0.00003051757) return 0.0;

    const double decibels = 20.0 * std::log10(amplitude);
    return std::max(0.0, 1.0 - decibels * zeroAmplitudeDbInv);
}

void Math::run() {
    while (!threadShouldExit()) {
        updateSampleRate(processor.getSampleRate());

        auto& octaves = processor.getOctaves();
        const int signalBlockSize = cqtEngine.getSignalBlockSize();

        if (signalBlockSize <= 0 || slidingWindows.empty()) {
            wait(5);
            continue;
        }

        int availableSamples = octaves[0].fifo.getNumReady();

        // --- DIAGNOSTIC LOGGING ---
        static uint32_t lastMathLog = 0;
        uint32_t nowMath = juce::Time::getMillisecondCounter();
        if (nowMath - lastMathLog > 1000) {
            lastMathLog = nowMath;
        }
        // --------------------------

        flushStaleAudioData(availableSamples);

        if (availableSamples >= 1024) {
            consumeAudioFromFifo();
            if (processor.settings.isShowNeedle) {
                processPitchDetection();
            }
            if (processor.settings.isShowEye
                || (processor.settings.isShowRoll && !processor.settings.isUseStftRoll)
            ) {
                processCqtAndEqualization();
            }
            if (processor.settings.isShowRoll && processor.settings.isUseStftRoll) {
                processStft();
            }
            publishResultsToUi();
        } else {
            wait(2);
        }
    }
}

void Math::flushStaleAudioData(int& availableSamples) {
    // --- THE LATENCY KILLER (FRAME DROPPING) ---
    // If the DSP math falls behind real-time, the FIFO backs up and creates massive visual latency.
    // If we have more than a few blocks waiting, instantly flush the old ones to catch up to live audio.

    if (availableSamples > 16384) {
        Util::debug("!!! FLUSHING STALE AUDIO !!! Dropping from " + juce::String(availableSamples) + " samples.");
        auto& octaves = processor.getOctaves();

        // Individually flush every octave to prevent Decimation Cascade Desync.
        for (int oct = 0; oct < PitchengaAudioProcessor::numOctaves; ++oct) {
            int ready = octaves[static_cast<size_t>(oct)].fifo.getNumReady();
            int keepWanted = 1024 >> oct;

            if (ready > keepWanted) {
                int drop = ready - keepWanted;
                int startOne, sizeOne, startTwo, sizeTwo;
                octaves[static_cast<size_t>(oct)].fifo.prepareToRead(drop, startOne, sizeOne, startTwo, sizeTwo);
                octaves[static_cast<size_t>(oct)].fifo.finishedRead(sizeOne + sizeTwo);
            }
        }
        availableSamples = octaves[0].fifo.getNumReady();

        //fixme: Is it needed?
        // Clear history buffers to prevent a massive broadband noise burst (the "CRT flicker" flash)
        // std::fill(rawAudioHistoryBuffer.begin(), rawAudioHistoryBuffer.end(), 0.0f);
        // for (auto& win : slidingWindows) {
            // std::fill(win.begin(), win.end(), 0.0f);
        // }
        // std::fill(pitchAnalysisBuffer.begin(), pitchAnalysisBuffer.end(), 0.0f);
    }
}

void Math::consumeAudioFromFifo() {
    auto& octaves = processor.getOctaves();
    const int signalBlockSize = cqtEngine.getSignalBlockSize();

    for (int oct = 0; oct < PitchengaAudioProcessor::numOctaves; ++oct) {
        auto& fifo = octaves[static_cast<size_t>(oct)].fifo;
        auto& buffer = octaves[static_cast<size_t>(oct)].buffer;
        auto& window = slidingWindows[static_cast<size_t>(oct)];

        // Calculate exactly how many samples this octave should process
        // Oct 0: 1024, Oct 1: 512, Oct 2: 256...
        const int samplesWanted = 1024 >> oct;
        int start1, size1, start2, size2;
        fifo.prepareToRead(samplesWanted, start1, size1, start2, size2);
        const int actualRead = size1 + size2;

        if (actualRead > 0) {
            // Shift the sliding window left
            std::memmove(
                window.data(),
                window.data() + actualRead,
                static_cast<size_t>(signalBlockSize - actualRead) * sizeof(float)
            );

            // Append the cleanly extracted data
            if (size1 > 0)
                std::copy(
                    buffer.begin() + start1,
                    buffer.begin() + (start1 + size1),
                    window.begin() + (static_cast<ptrdiff_t>(signalBlockSize) - actualRead)
                );
            if (size2 > 0)
                std::copy(
                    buffer.begin() + start2,
                    buffer.begin() + (start2 + size2),
                    window.begin() + (static_cast<ptrdiff_t>(signalBlockSize) - actualRead) + size1
                );

            if (oct == 0) {
                // Shift the pitch history left
                std::memmove(
                    rawAudioHistoryBuffer.data(),
                    rawAudioHistoryBuffer.data() + actualRead,
                    static_cast<size_t>(32768 - actualRead) * sizeof(float)
                );

                // Append the exact same raw audio we just pulled from the FIFO
                if (size1 > 0)
                    std::copy(
                        buffer.begin() + start1,
                        buffer.begin() + (start1 + size1),
                        rawAudioHistoryBuffer.begin() + (32768 - actualRead)
                    );
                if (size2 > 0)
                    std::copy(
                        buffer.begin() + start2,
                        buffer.begin() + (start2 + size2),
                        rawAudioHistoryBuffer.begin() + (32768 - actualRead) + size1
                    );
            }
            fifo.finishedRead(actualRead);
        }
    }
}

void Math::processPitchDetection() {
    // --- Pitch Detection (Using latest 4096 samples) ---
    if (pitchDetector != nullptr) {
        const double samplingFreq = processor.getSampleRate() > 0 ? processor.getSampleRate() : 44100.0;
        // Force weak fundamentals above the MPM clarity threshold.
        std::copy(rawAudioHistoryBuffer.end() - 4096, rawAudioHistoryBuffer.end(), pitchAnalysisBuffer.begin());
        juce::FloatVectorOperations::multiply(pitchAnalysisBuffer.data(), 12.0f, 4096);
        const float detectedPitch = pitchDetector->getPitch(pitchAnalysisBuffer, static_cast<int>(samplingFreq));
        // Update the atomic variable for the UI timer to read
        processor.currentPitchHz.store(detectedPitch, std::memory_order_relaxed);
    }
}

void Math::processCqtAndEqualization() {
    const int binsPerOctave = cqtEngine.getBinsPerOctave();
    const int signalBlockSize = cqtEngine.getSignalBlockSize();

    // --- CQT Processing (For Eye) ---
    for (int oct = 0; oct < PitchengaAudioProcessor::numOctaves; ++oct) {
        const auto& window = slidingWindows[static_cast<size_t>(oct)];
        if (window.size() == static_cast<size_t>(signalBlockSize)) {
            cqtEngine.transform(window, cqtSpectrum);
            const int startIndex = (PitchengaAudioProcessor::numOctaves - 1 - oct) * binsPerOctave;
            for (size_t i = 0; i < cqtSpectrum.size(); ++i) {
                const double amplitude = std::abs(cqtSpectrum[i]) * inputGain;
                amplitudeSpectrumDb[static_cast<size_t>(startIndex) + i] = amplitudeToDbRescaled(amplitude);
            }
        }
    }

    const auto& smoothedSpectrum = amplitudeSpectrumDb;
    const auto& detectedPitchClasses = pitchClassDetector->detectPitchClasses(smoothedSpectrum);
    const auto& equalizedPitchClasses = spectralEqualizer->filter(detectedPitchClasses);

    std::ranges::fill(octaveBins, 0.0);
    for (int i = 0; i < binsPerOctave; ++i) {
        double maximumValue = 0.0;
        for (int j = i; j < static_cast<int>(equalizedPitchClasses.size()); j += binsPerOctave) {
            maximumValue = std::max(maximumValue, equalizedPitchClasses[static_cast<size_t>(j)]);
        }
        octaveBins[static_cast<size_t>(i)] = maximumValue;
    }
    const auto& smoothedCircleData = octaveBinSmoother->smooth(octaveBins);

    currentEyeData = smoothedCircleData;
    currentRollData = equalizedPitchClasses;
}

void Math::publishResultsToUi() {
    // --- Push Results to UI ---
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    if (eyeResults.size() == currentEyeData.size()) {
        std::ranges::copy(currentEyeData, eyeResults.begin());
    }

    // fixme remove Old line buffer logic for discrete CQT bins
    if (rollResults.size() == currentRollData.size()) {
        std::ranges::copy(currentRollData, rollResults.begin());
    }

    uiRollPeaks = currentRollPeaks;

    newDataAvailable.store(true, std::memory_order_release);
}

void Math::getRollPeaks(std::vector<SpectralPeak>& destinationArray) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    destinationArray = uiRollPeaks;
}

void Math::getCircleResults(std::vector<double>& destinationArray) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    if (destinationArray.size() != eyeResults.size()) {
        destinationArray.resize(eyeResults.size());
    }
    std::copy(eyeResults.begin(), eyeResults.end(), destinationArray.begin());
}

void Math::getLineResults(std::vector<double>& destinationArray) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    if (destinationArray.size() != rollResults.size()) destinationArray.resize(rollResults.size());
    std::copy(rollResults.begin(), rollResults.end(), destinationArray.begin());
}
