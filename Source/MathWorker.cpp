#include "MathWorker.h"

MathWorker::MathWorker(PitchengaAudioProcessor& processorToUse)
    : Thread("VisualizeWorker"), audioProcessor(processorToUse) {
    setupBuffers();
}

MathWorker::~MathWorker() {
    stopThread(2000);
}

void MathWorker::setupBuffers() {
    setupCqtEngine();
    setupCqtBuffers();
    setupPitchDetection();
}

void MathWorker::setupCqtEngine() {
    CqtEngine::Config config;
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

void MathWorker::setupCqtBuffers() {
    const int totalBins = cqtEngine.getTotalBins();
    const int binsPerOctave = cqtEngine.getBinsPerOctave();
    const auto signalBlockSize = static_cast<size_t>(cqtEngine.getSignalBlockSize());

    workingBuffer.assign(signalBlockSize, 0.0f);
    // Initialize sliding windows for each octave
    slidingWindows.assign(PitchengaAudioProcessor::numOctaves, std::vector(signalBlockSize, 0.0f));

    cqtSpectrum.resize(static_cast<size_t>(cqtEngine.getKernelBins()));
    amplitudeSpectrumDb.assign(static_cast<size_t>(totalBins), 0.0);
    octaveBins.assign(static_cast<size_t>(binsPerOctave), 0.0);

    {
        const juce::CriticalSection::ScopedLockType lock(resultLock);
        circleVisualizerResults.assign(static_cast<size_t>(binsPerOctave), 0.0);
        lineVisualizerResults.assign(static_cast<size_t>(totalBins), 0.0);
    }
}

void MathWorker::setupPitchDetection() {
    const double samplingFreq = audioProcessor.getSampleRate() > 0 ? audioProcessor.getSampleRate() : 44100.0;

    // --- Pitch Setup ---
    pitchDetector = std::make_unique<adamski::PitchMPM>(static_cast<int>(samplingFreq), 4096);
    rawAudioHistoryBuffer.assign(8192, 0.0f);
    pitchAnalysisBuffer.assign(4096, 0.0f);
}

void MathWorker::updateSampleRate(const double newSampleRate) {
    if (newSampleRate > 0 && std::abs(newSampleRate - cqtEngine.getConfig().samplingFreq) > 0.01) {
        setupBuffers();
    }
}

double MathWorker::amplitudeToDbRescaled(const double amplitude) {
    // Exact port of HarmonEye's DecibelCalculator.java
    constexpr double zeroAmplitudeDb = -90.30899869919436;
    constexpr double zeroAmplitudeDbInv = 1.0 / zeroAmplitudeDb;

    if (amplitude <= 0.00003051757) return 0.0;

    const double decibels = 20.0 * std::log10(amplitude);
    return std::max(0.0, 1.0 - decibels * zeroAmplitudeDbInv);
}

void MathWorker::run() {
    while (!threadShouldExit()) {
        updateSampleRate(audioProcessor.getSampleRate());

        auto& octaves = audioProcessor.getOctaves();
        const int signalBlockSize = cqtEngine.getSignalBlockSize();

        if (signalBlockSize <= 0 || slidingWindows.empty()) {
            wait(5);
            continue;
        }

        int availableSamples = octaves[0].fifo.getNumReady();

        flushStaleAudioData(availableSamples);

        // Process exactly ONE 1024-sample block per wake-up
        if (availableSamples >= 1024) {
            consumeAudioFromFifo();
            processPitchDetection();
            processCqtAndEqualization();
            publishResultsToUi();
        }
        wait(1);
    }
}

void MathWorker::flushStaleAudioData(int& availableSamples) {
    // --- THE LATENCY KILLER (FRAME DROPPING) ---
    // If the DSP math falls behind real-time, the FIFO backs up and creates massive visual latency.
    // If we have more than a few blocks waiting, instantly flush the old ones to catch up to live audio.
    if (availableSamples > 4096) {
        int samplesToDrop = availableSamples - 1024;
        samplesToDrop = (samplesToDrop / 1024) * 1024; // Round down to perfect 1024 chunks

        auto& octaves = audioProcessor.getOctaves();
        for (int oct = 0; oct < PitchengaAudioProcessor::numOctaves; ++oct) {
            const int dropForOctave = samplesToDrop >> oct; // Accurately drop decimated amounts
            int startOne, sizeOne, startTwo, sizeTwo;
            octaves[static_cast<size_t>(oct)].fifo.prepareToRead(dropForOctave, startOne, sizeOne, startTwo, sizeTwo);
            octaves[static_cast<size_t>(oct)].fifo.finishedRead(sizeOne + sizeTwo);
        }
        availableSamples = octaves[0].fifo.getNumReady(); // Recalculate what's left
    }
}

void MathWorker::consumeAudioFromFifo() {
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
                    static_cast<size_t>(8192 - actualRead) * sizeof(float)
                );

                // Append the exact same raw audio we just pulled from the FIFO
                if (size1 > 0)
                    std::copy(
                        buffer.begin() + start1,
                        buffer.begin() + (start1 + size1),
                        rawAudioHistoryBuffer.begin() + (8192 - actualRead)
                    );
                if (size2 > 0)
                    std::copy(
                        buffer.begin() + start2,
                        buffer.begin() + (start2 + size2),
                        rawAudioHistoryBuffer.begin() + (8192 - actualRead) + size1
                    );
            }
            fifo.finishedRead(actualRead);
        }
    }
}

void MathWorker::processPitchDetection() {
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

void MathWorker::processCqtAndEqualization() {
    const int binsPerOctave = cqtEngine.getBinsPerOctave();
    const int signalBlockSize = cqtEngine.getSignalBlockSize();

    // --- CQT Processing (For CircleViz) ---
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

    // Pre-analyze smoothing does not seem to improve anything for the circle, but it smears the spectrogram
    // const auto& smoothedSpectrum = allBinSmoother->smooth(amplitudeSpectrumDb);
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

    currentCircleVizData = smoothedCircleData;
    currentLineVizData = equalizedPitchClasses;
}

void MathWorker::publishResultsToUi() {
    // --- Push Results to UI ---
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    if (circleVisualizerResults.size() == currentCircleVizData.size()) {
        std::ranges::copy(currentCircleVizData, circleVisualizerResults.begin());
    }
    if (lineVisualizerResults.size() == currentLineVizData.size()) {
        std::ranges::copy(currentLineVizData, lineVisualizerResults.begin());
    }
    newDataAvailable.store(true, std::memory_order_release);
}

void MathWorker::getCircleResults(std::vector<double>& destinationArray) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    if (destinationArray.size() != circleVisualizerResults.size()) destinationArray.resize(
        circleVisualizerResults.size()
    );
    std::copy(circleVisualizerResults.begin(), circleVisualizerResults.end(), destinationArray.begin());
}

void MathWorker::getLineResults(std::vector<double>& destinationArray) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    if (destinationArray.size() != lineVisualizerResults.size()) destinationArray.resize(lineVisualizerResults.size());
    std::copy(lineVisualizerResults.begin(), lineVisualizerResults.end(), destinationArray.begin());
}
