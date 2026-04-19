#include "MathWorker.h"

MathWorker::MathWorker(PitchengaAudioProcessor& processorToUse)
    : Thread("VisualizeWorker"), audioProcessor(processorToUse) {
    setupBuffers();
}

MathWorker::~MathWorker() {
    stopThread(2000);
}

void MathWorker::setupBuffers() {
    CqtEngine::Config config;
    config.octaves = PitchengaAudioProcessor::numOctaves;
    config.samplingFreq = audioProcessor.getSampleRate() > 0 ? audioProcessor.getSampleRate() : 44100.0;
    cqtEngine.updateConfig(config);
    cqtEngine.init();

    const int totalBins = cqtEngine.getTotalBins();
    const int binsPerOctave = cqtEngine.getBinsPerOctave();
    const auto signalBlockSize = static_cast<size_t>(cqtEngine.getSignalBlockSize());

    pitchClassDetector = std::make_unique<HarmonicPatternPitchClassDetector>(binsPerOctave, config.binsPerSemitone);
    spectralEqualizer = std::make_unique<SpectralEqualizer>(totalBins, 30);
    allBinSmoother = std::make_unique<ExpSmoother>(totalBins, 0.2);
    octaveBinSmoother = std::make_unique<ExpSmoother>(binsPerOctave, 0.05);

    workingBuffer.assign(signalBlockSize, 0.0f);
    slidingWindows.assign(PitchengaAudioProcessor::numOctaves, std::vector(signalBlockSize, 0.0f));

    cqtSpectrum.resize(static_cast<size_t>(cqtEngine.getKernelBins()));
    amplitudeSpectrumDb.assign(static_cast<size_t>(totalBins), 0.0);
    octaveBins.assign(static_cast<size_t>(binsPerOctave), 0.0);

    // - Bumped to 16384 samples for higher bass resolution.
    fastFourierTransform = std::make_unique<juce::dsp::FFT>(fastFourierTransformOrder);
    windowingFunction = std::make_unique<juce::dsp::WindowingFunction<float>>(
        fastFourierTransformSize, juce::dsp::WindowingFunction<float>::hamming
    );
    rawAudioHistoryBuffer.assign(fastFourierTransformSize, 0.0f);
    windowedFftBuffer.assign(fastFourierTransformSize, 0.0f);
    complexFftWorkspace.assign(fastFourierTransformSize * 2, 0.0f);
    linearFftMagnitudes.assign(static_cast<size_t>(totalBins), 0.0);

    // --- Pitch Setup ---
    pitchDetector = std::make_unique<adamski::PitchMPM>(static_cast<int>(config.samplingFreq), 4096);
    pitchAnalysisBuffer.assign(4096, 0.0f);

    {
        const juce::CriticalSection::ScopedLockType lock(resultLock);
        circleVisualizerResults.assign(static_cast<size_t>(binsPerOctave), 0.0);
        lineVisualizerResults.assign(static_cast<size_t>(totalBins), 0.0);
    }
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
    return std::max(0.0, 1.0 - (decibels * zeroAmplitudeDbInv));
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
            processFftForLinearVisualizer();
            publishResultsToUserInterface();
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
        int startOne, sizeOne, startTwo, sizeTwo;
        fifo.prepareToRead(samplesWanted, startOne, sizeOne, startTwo, sizeTwo);
        const int actualRead = sizeOne + sizeTwo;

        if (actualRead > 0) {
            // Shift the sliding window left
            std::memmove(
                window.data(),
                window.data() + actualRead,
                static_cast<size_t>(signalBlockSize - actualRead) * sizeof(float)
            );

            // Append the cleanly extracted data
            if (sizeOne > 0) std::copy(
                buffer.begin() + startOne,
                buffer.begin() + (startOne + sizeOne),
                window.begin() + (static_cast<ptrdiff_t>(signalBlockSize) - actualRead)
            );
            if (sizeTwo > 0) std::copy(
                buffer.begin() + startTwo,
                buffer.begin() + (startTwo + sizeTwo),
                window.begin() + (static_cast<ptrdiff_t>(signalBlockSize) - actualRead) + sizeOne
            );

            // Maintain the global raw audio buffer for FFT and Pitch
            if (oct == 0) {
                // Shift the pitch history left
                std::memmove(
                    rawAudioHistoryBuffer.data(),
                    rawAudioHistoryBuffer.data() + actualRead,
                    static_cast<size_t>(fastFourierTransformSize - actualRead) * sizeof(float)
                );

                // Append the exact same raw audio we just pulled from the FIFO
                if (sizeOne > 0) std::copy(
                    buffer.begin() + startOne,
                    buffer.begin() + (startOne + sizeOne),
                    rawAudioHistoryBuffer.begin() + (fastFourierTransformSize - actualRead)
                );
                if (sizeTwo > 0) std::copy(
                    buffer.begin() + startTwo,
                    buffer.begin() + (startTwo + sizeTwo),
                    rawAudioHistoryBuffer.begin() + (fastFourierTransformSize - actualRead) + sizeOne
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

    // Pass to class scope for publishing later
    circleVisualizerResults = smoothedCircleData;
}

void MathWorker::processFftForLinearVisualizer() {
    const int totalBins = cqtEngine.getTotalBins();

    // --- FFT Processing (For LineViz) ---
    // Copy and apply window to purely real data
    std::copy(rawAudioHistoryBuffer.begin(), rawAudioHistoryBuffer.end(), windowedFftBuffer.begin());
    windowingFunction->multiplyWithWindowingTable(windowedFftBuffer.data(), fastFourierTransformSize);

    // Safely unpack into Complex memory layout [Real, Imaginary, Real, Imaginary...]
    for (int i = 0; i < fastFourierTransformSize; ++i) {
        complexFftWorkspace[static_cast<size_t>(i * 2)] = windowedFftBuffer[static_cast<size_t>(i)];
        complexFftWorkspace[static_cast<size_t>(i * 2 + 1)] = 0.0f;
    }

    fastFourierTransform->performFrequencyOnlyForwardTransform(complexFftWorkspace.data());

    const double sampleRate = cqtEngine.getConfig().samplingFreq;
    constexpr double fftNormalizationFactor = 2.0 / (static_cast<double>(fastFourierTransformSize) * 0.54);

    // Map linear FFT to Pitchenga Grid using Hybrid Fractional Interpolation
    for (int i = 0; i < totalBins; ++i) {
        const double centerFrequency = cqtEngine.centerFreq(i);
        const double halfStepRatio = std::pow(2.0, 0.5 / static_cast<double>(cqtEngine.getConfig().binsPerSemitone));

        const double lowerFrequency = centerFrequency / halfStepRatio;
        const double upperFrequency = centerFrequency * halfStepRatio;

        const double exactLower = (lowerFrequency * static_cast<double>(fastFourierTransformSize)) / sampleRate;
        const double exactUpper = (upperFrequency * static_cast<double>(fastFourierTransformSize)) / sampleRate;

        double maximumEnergyInBand = 0.0;

        if (exactUpper - exactLower < 1.0) {
            // LOW FREQUENCIES: Sub-bin resolution needed. Apply linear interpolation between the two closest FFT bins.
            const double exactCenter = (centerFrequency * static_cast<double>(fastFourierTransformSize)) / sampleRate;

            // Fix: Enforce reading from Bin 1+ to escape DC offset noise at Bin 0
            const int index1 = std::max(1, static_cast<int>(exactCenter));
            const int index2 = std::min(index1 + 1, fastFourierTransformSize / 2 - 1);
            const double fraction = exactCenter - index1;

            const double mag1 = complexFftWorkspace[static_cast<size_t>(index1)];
            const double mag2 = complexFftWorkspace[static_cast<size_t>(index2)];

            maximumEnergyInBand = mag1 + fraction * (mag2 - mag1);
        } else {
            // HIGH FREQUENCIES: A visual bin spans multiple FFT bins. Pick the peak.

            // Fix: Enforce reading from Bin 1+ to escape DC offset noise at Bin 0
            const int lowerIndex = std::max(1, static_cast<int>(exactLower));
            const int upperIndex = static_cast<int>(exactUpper);

            for (int binIndex = lowerIndex; binIndex <= upperIndex && binIndex < (fastFourierTransformSize / 2); ++binIndex) {
                const double energy = complexFftWorkspace[static_cast<size_t>(binIndex)];
                if (energy > maximumEnergyInBand) {
                    maximumEnergyInBand = energy;
                }
            }
        }

        // Apply the normalization factor to shrink the massive FFT numbers back down to 0.0 - 1.0
        // I have re-added the inputGain here so it mathematically matches CircleViz!
        const double normalizedAmplitude = maximumEnergyInBand * fftNormalizationFactor * inputGain;
        linearFftMagnitudes[static_cast<size_t>(i)] = amplitudeToDbRescaled(normalizedAmplitude);
    }
}

void MathWorker::publishResultsToUserInterface() {
    // --- Push Results to UI ---
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    // (circleVisualizerResults is already populated locally inside processCqtAndEqualization)
    if (lineVisualizerResults.size() == linearFftMagnitudes.size()) {
        std::ranges::copy(linearFftMagnitudes, lineVisualizerResults.begin());
    }
    newDataAvailable.store(true, std::memory_order_release);
}

void MathWorker::getCircleResults(std::vector<double>& destinationArray) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    if (destinationArray.size() != circleVisualizerResults.size()) destinationArray.resize(circleVisualizerResults.size());
    std::copy(circleVisualizerResults.begin(), circleVisualizerResults.end(), destinationArray.begin());
}

void MathWorker::getLineResults(std::vector<double>& destinationArray) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    if (destinationArray.size() != lineVisualizerResults.size()) destinationArray.resize(lineVisualizerResults.size());
    std::copy(lineVisualizerResults.begin(), lineVisualizerResults.end(), destinationArray.begin());
}