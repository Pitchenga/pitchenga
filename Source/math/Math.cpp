#include "Math.h"
#include "../Util.h"
#include "../Tone.h"

Math::Math(PitchengaAudioProcessor& processorToUse)
    : Thread("VisualizeWorker"), audioProcessor(processorToUse) {
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
    config.samplingFreq = audioProcessor.getSampleRate() > 0 ? audioProcessor.getSampleRate() : 44100.0;
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
    const double samplingFreq = audioProcessor.getSampleRate() > 0 ? audioProcessor.getSampleRate() : 44100.0;

    // --- Pitch Setup ---
    pitchDetector = std::make_unique<adamski::PitchMPM>(static_cast<int>(samplingFreq), 4096);
    rawAudioHistoryBuffer.assign(32768, 0.0f);
    pitchAnalysisBuffer.assign(4096, 0.0f);
}

void Math::setupStft() {
    const double samplingFreq = audioProcessor.getSampleRate() > 0 ? audioProcessor.getSampleRate() : 44100.0;
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
    //fixme: targetTimeMs is not used
    double targetTimeMs = juce::Time::getMillisecondCounterHiRes();

    while (!threadShouldExit()) {
        updateSampleRate(audioProcessor.getSampleRate());

        auto& octaves = audioProcessor.getOctaves();
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
            processPitchDetection();
            processCqtAndEqualization();
            processStft();
            publishResultsToUi();

            // Strict High-Res Pacing.
            // Batch-processing overwrites the UI buffer faster than 48Hz, causing massive skipped frames (steam gaps).
            // By mathematically pacing the thread to exact real-time playback, we guarantee the UI catches EVERY frame.
            double sr = audioProcessor.getSampleRate() > 0 ? audioProcessor.getSampleRate() : 44100.0;
            double frameDurationMs = (1024.0 / sr) * 1000.0;

            targetTimeMs += frameDurationMs;
            double now = juce::Time::getMillisecondCounterHiRes();

            if (targetTimeMs > now) {
                int sleepTime = static_cast<int>(targetTimeMs - now);
                if (sleepTime > 0) wait(sleepTime);
            } else {
                // If we fall behind or the DAW is paused, snap the timeline back to reality.
                targetTimeMs = now;
            }
        } else {
            wait(2);
            // Keep the target time anchored to reality while waiting for the DAW to play
            targetTimeMs = juce::Time::getMillisecondCounterHiRes();
        }
        wait(1);
    }
}

void Math::flushStaleAudioData(int& availableSamples) const {
    // --- THE LATENCY KILLER (FRAME DROPPING) ---
    // If the DSP math falls behind real-time, the FIFO backs up and creates massive visual latency.
    // If we have more than a few blocks waiting, instantly flush the old ones to catch up to live audio.

    if (availableSamples > 16384) {
        Util::debug("!!! FLUSHING STALE AUDIO !!! Dropping from " + juce::String(availableSamples) + " samples.");
        auto& octaves = audioProcessor.getOctaves();

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
    }
}

void Math::consumeAudioFromFifo() {
    auto& octaves = audioProcessor.getOctaves();
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
        // Force weak fundamentals above the MPM clarity threshold.
        std::copy(rawAudioHistoryBuffer.end() - 4096, rawAudioHistoryBuffer.end(), pitchAnalysisBuffer.begin());
        juce::FloatVectorOperations::multiply(pitchAnalysisBuffer.data(), 12.0f, 4096);
        const float detectedPitch = pitchDetector->getPitch(pitchAnalysisBuffer.data());
        // Update the atomic variable for the UI timer to read
        audioProcessor.currentPitchHz.store(detectedPitch, std::memory_order_relaxed);
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

    pumpSteam();

    newDataAvailable.store(true, std::memory_order_release);
}

void Math::getRollPeaks(std::vector<SpectralPeak>& destinationArray) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    destinationArray = uiRollPeaks;
}

void Math::getCircleResults(std::vector<double>& destinationArray) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    if (destinationArray.size() != eyeResults.size()) destinationArray.resize(
        eyeResults.size()
    );
    std::copy(eyeResults.begin(), eyeResults.end(), destinationArray.begin());
}

void Math::getLineResults(std::vector<double>& destinationArray) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    if (destinationArray.size() != rollResults.size()) destinationArray.resize(rollResults.size());
    std::copy(rollResults.begin(), rollResults.end(), destinationArray.begin());
}

void Math::setSteamSize(int width, int height) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    if (width != steamWidth || height != steamHeight) {
        steamWidth = width;
        steamHeight = height;
        if (steamWidth > 0 && steamHeight > 0) {
            steamImage = juce::Image(juce::Image::ARGB, steamWidth, steamHeight, true);
            steamScrollOffset = 0;
        } else {
            steamImage = juce::Image();
        }
    }
}

void Math::getSteamImage(juce::Image& destinationImage, int& scrollOffset) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    destinationImage = steamImage;
    scrollOffset = steamScrollOffset;
}

void Math::pumpSteam() {
    if (!audioProcessor.settings.showSteam || !steamImage.isValid()) return;

    const int width = steamWidth;
    const int height = steamHeight;
    if (width <= 0 || height <= 0) return;

    const int speedPx = static_cast<int>(steamSpeedPxPerFrame);

    // Advance the scroll offset and wrap it like a treadmill
    steamScrollOffset = (steamScrollOffset + speedPx) % height;

    // Calculate where in the image memory the new row should be drawn
    const int drawY = (height - speedPx + steamScrollOffset) % height;

    juce::Graphics graphics(steamImage);

    // Clear the new row first to prevent ghosting from previous treadmill cycles
    graphics.setColour(juce::Colours::black);
    graphics.fillRect(0, drawY, width, speedPx);

    if (currentRollPeaks.empty()) return;

    const float sr = audioProcessor.getSampleRate() > 0.0 ? static_cast<float>(audioProcessor.getSampleRate()) : 44100.0f;
    const float binResHz = sr / 32768.0f;
    const float fWidth = static_cast<float>(width);
    const float midiRangeInv = 1.0f / (maxMidiNote - minMidiNote);
    const bool doDynamicStem = enableDynamicStemWidth;

    // Extreme Math Optimization: Pre-calculate the derivative of the MIDI scale
    // to bypass calling std::log2 multiple times per peak.
    const float derivativeFactor = fWidth * midiRangeInv * 17.3123405f * binResHz;

    for (const auto& peak : currentRollPeaks) {
        if (peak.rawMagnitude > 0.05f) {
            // Prevents rendering absolute silence noise

            // Inline the math for the primary position calculation
            const float midi = 69.0f + 12.0f * std::log2(peak.frequencyHz / 440.0f);
            const float xPos = fWidth * ((midi - minMidiNote) * midiRangeInv);

            // Fast bounds culling
            if (xPos >= -10.0f && xPos <= fWidth + 10.0f) {
                // Configurable razor-sharp stems for the Steam
                float stemWidthPixels = 4.0f;
                if (doDynamicStem) {
                    // Use the fast derivative approximation instead of another heavy log2
                    const float nextX = xPos + (derivativeFactor / peak.frequencyHz);
                    // +1.0f forces deliberate sub-pixel overlap to completely kill rendering gaps
                    stemWidthPixels = std::max(1.0f, (nextX - xPos) + 1.0f);
                }

                // Fast continuous modulus (replaces heavy std::fmod)
                float continuousChroma = midi;
                while (continuousChroma >= 12.0f) continuousChroma -= 12.0f;
                while (continuousChroma < 0.0f) continuousChroma += 12.0f;

                const juce::Colour baseColor = Tone::getContinuousColor(continuousChroma);
                constexpr float undimmingGain = 1.6f;

                const float clampedMag = std::min(1.0f, peak.rawMagnitude * undimmingGain);
                const juce::Colour color = juce::Colours::black.interpolatedWith(baseColor, clampedMag);

                graphics.setColour(color);
                graphics.fillRect(
                    xPos - (stemWidthPixels * 0.5f),
                    static_cast<float>(drawY),
                    stemWidthPixels,
                    static_cast<float>(speedPx)
                );
            }
        }
    }
}