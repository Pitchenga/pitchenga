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
    const double samplingFreq = processor.getSampleRate() > 0 ? processor.getSampleRate() : 44100.0;

    // Pitch Setup - We use a 1024 buffer for decimated audio (equivalent to 4096 at full rate)
    pitchDetector = std::make_unique<sevagh::PitchDetector<float>>(1024);
    rawAudioHistoryBuffer.assign(32768, 0.0f);
    pitchAnalysisBuffer.assign(4096, 0.0f);

    // Decimation setup (G1 and below support)
    decimatedHistoryBuffer.assign(8192, 0.0f);
    decimatedAnalysisBuffer.assign(1024, 0.0f);
    decimationCounter = 0;

    // 300Hz Low-Pass filter to remove harmonics before decimation
    pitchLowpass.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(samplingFreq, 300.0f);
    pitchLowpass.reset();
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

        flushStaleAudioData(availableSamples);

        if (availableSamples >= 1024) {
            consumeAudioFromFifo();
            if (processor.settings.isShowNeedle) {
                processPitchDetection();
            }
            if (processor.settings.isShowEye
                || (processor.settings.isShowRoll && !processor.settings.isUseRollStft)
            ) {
                processCqtAndEqualization();
            }
            if (processor.settings.isShowRoll && processor.settings.isUseRollStft) {
                processStft();
            }
            publishResultsToUi();
        } else {
            wait(2);
        }
    }
}

void Math::flushStaleAudioData(int& availableSamples) {
    // The latency killer (frame dropping)
    // If the Dsp math falls behind real-time, the Fifo backs up and creates massive visual latency.
    // If we have more than a few blocks waiting, instantly flush the old ones to catch up to live audio.

    if (availableSamples > 16384) {
        Util::debug("!!! FLUSHING STALE AUDIO !!! Dropping from " + juce::String(availableSamples) + " samples.");
        auto& octaves = processor.getOctaves();

        // Individually flush every octave to prevent Decimation Cascade Desync.
        for (int oct = 0; oct < PitchengaAudioProcessor::numOctaves; ++oct) {
            int ready = octaves[static_cast<size_t>(oct)].fifo.getNumReady();
            const int keepWanted = 1024 >> oct;

            if (ready > keepWanted) {
                const int drop = ready - keepWanted;
                int startOne, sizeOne, startTwo, sizeTwo;
                octaves[static_cast<size_t>(oct)].fifo.prepareToRead(drop, startOne, sizeOne, startTwo, sizeTwo);
                octaves[static_cast<size_t>(oct)].fifo.finishedRead(sizeOne + sizeTwo);
            }
        }
        availableSamples = octaves[0].fifo.getNumReady();
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

                // Decimate for Pitch Detection (4x downsampling with 300Hz LP filter)
                const int decimatedSamplesAdded = actualRead / 4;
                if (decimatedSamplesAdded > 0) {
                    std::memmove(
                        decimatedHistoryBuffer.data(),
                        decimatedHistoryBuffer.data() + decimatedSamplesAdded,
                        static_cast<size_t>(8192 - decimatedSamplesAdded) * sizeof(float)
                    );

                    float* writePtr = decimatedHistoryBuffer.data() + (8192 - decimatedSamplesAdded);
                    int writeIdx = 0;
                    for (int i = 0; i < actualRead; ++i) {
                        const float s = (i < size1) ? buffer[static_cast<size_t>(start1 + i)] : buffer[static_cast<size_t>(start2 + (i - size1))];
                        const float filtered = pitchLowpass.processSample(s);
                        if (++decimationCounter >= 4) {
                            decimationCounter = 0;
                            if (writeIdx < decimatedSamplesAdded) {
                                writePtr[writeIdx++] = filtered;
                            }
                        }
                    }
                }
            }
            fifo.finishedRead(actualRead);
        }
    }
}

void Math::processPitchDetection() {
    // Pitch Detection (Using latest 4096 samples)
    if (pitchDetector != nullptr) {
        const double samplingFreq = processor.getSampleRate() > 0 ? processor.getSampleRate() : 44100.0;
        const double decimatedSamplingFreq = samplingFreq / 4.0;
        
        // Feed the latest block (1024 decimated samples) to the pitch detector
        std::copy(decimatedHistoryBuffer.end() - 1024, decimatedHistoryBuffer.end(), decimatedAnalysisBuffer.begin());

        //fixme: Is it needed?
        // Apply gain to the analysis buffer to ensure signal is strong enough for peak picking
        // juce::FloatVectorOperations::multiply(pitchAnalysisBuffer.data(), 15.0f, 4096);
        const float detectedPitch = pitchDetector->getPitch(decimatedAnalysisBuffer, static_cast<int>(decimatedSamplingFreq));
        
        // if (detectedPitch > 0.0f) {
            // Util::debug("Math Thread - Detected Pitch: " + juce::String(detectedPitch) + " Hz (Decimated)");
        // }
        
        // Update the atomic variable for the UI timer to read
        processor.currentPitchHz.store(detectedPitch, std::memory_order_relaxed);
    }
}

void Math::processCqtAndEqualization() {
    const int binsPerOctave = cqtEngine.getBinsPerOctave();
    const int signalBlockSize = cqtEngine.getSignalBlockSize();

    // CQT Processing (For Eye)
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
    // Push Results to UI
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    if (eyeResults.size() == currentEyeData.size()) {
        std::ranges::copy(currentEyeData, eyeResults.begin());
    }

    if (rollResults.size() == currentRollData.size()) {
        std::ranges::copy(currentRollData, rollResults.begin());
    }

    uiRollPeaks = currentRollPeaks;

    newDataAvailable.store(true, std::memory_order_release);
}

void Math::getRollStftResults(std::vector<SpectralPeak>& destinationArray) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    destinationArray = uiRollPeaks;
}

void Math::getEyeResults(std::vector<double>& destinationArray) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    if (destinationArray.size() != eyeResults.size()) {
        destinationArray.resize(eyeResults.size());
    }
    std::copy(eyeResults.begin(), eyeResults.end(), destinationArray.begin());
}

void Math::getRollCqtResults(std::vector<double>& destinationArray) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    if (destinationArray.size() != rollResults.size()) destinationArray.resize(rollResults.size());
    std::copy(rollResults.begin(), rollResults.end(), destinationArray.begin());
}
