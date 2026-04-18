#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
#include <pitch_detector/pitch_detector.h>


// --- CqtWorkerThread Implementation ---

PitchengaAudioProcessorEditor::CqtWorkerThread::CqtWorkerThread(PitchengaAudioProcessor& p)
    : Thread("CqtWorker"), processor(p) {
    setupBuffers();
}

void PitchengaAudioProcessorEditor::CqtWorkerThread::setupBuffers() {
    CqtEngine::Config config;
    config.octaves = PitchengaAudioProcessor::numOctaves;
    config.samplingFreq = processor.getSampleRate() > 0 ? processor.getSampleRate() : 44100.0;
    cqt.updateConfig(config);
    cqt.init();

    const int totalBins = cqt.getTotalBins();
    const int binsPerOctave = cqt.getBinsPerOctave();
    const size_t signalBlockSize = static_cast<size_t>(cqt.getSignalBlockSize());

    pcDetector = std::make_unique<HarmonicPatternPitchClassDetector>(binsPerOctave, config.binsPerSemitone);
    spectralEqualizer = std::make_unique<SpectralEqualizer>(totalBins, 30);
    allBinSmoother = std::make_unique<ExpSmoother>(totalBins, 0.2);
    octaveBinSmoother = std::make_unique<ExpSmoother>(binsPerOctave, 0.2);

    workBuffer.assign(signalBlockSize, 0.0f);

    // Initialize sliding windows for each octave
    slidingWindows.assign(
        PitchengaAudioProcessor::numOctaves,
        std::vector(signalBlockSize, 0.0f)
    );

    cqtSpectrum.resize(static_cast<size_t>(cqt.getKernelBins()));
    amplitudeSpectrumDb.assign(static_cast<size_t>(totalBins), 0.0);
    octaveBins.assign(static_cast<size_t>(binsPerOctave), 0.0);

    {
        const juce::CriticalSection::ScopedLockType lock(resultLock);
        results.assign(static_cast<size_t>(binsPerOctave), 0.0);
    }
}

void PitchengaAudioProcessorEditor::CqtWorkerThread::updateSampleRate(double newSampleRate) {
    if (newSampleRate > 0 && std::abs(newSampleRate - cqt.getConfig().samplingFreq) > 0.01) {
        setupBuffers();
    }
}

double PitchengaAudioProcessorEditor::CqtWorkerThread::amplitudeToDbRescaled(double amplitude) {
    // Exact port of HarmonEye's DecibelCalculator.java
    constexpr double zeroAmplitudeDb = -90.30899869919436;
    constexpr double zeroAmplitudeDbInv = 1.0 / zeroAmplitudeDb;

    if (amplitude <= 0.00003051757) return 0.0;

    double db = 20.0 * std::log10(amplitude);
    return std::max(0.0, 1.0 - (db * zeroAmplitudeDbInv));
}

void PitchengaAudioProcessorEditor::CqtWorkerThread::run() {
    while (!threadShouldExit()) {
        updateSampleRate(processor.getSampleRate());

        auto& octaves = processor.getOctaves();
        const int signalBlockSize = cqt.getSignalBlockSize();
        const int binsPerOctave = cqt.getBinsPerOctave();

        if (signalBlockSize <= 0 || slidingWindows.empty()) {
            wait(5);
            continue;
        }

        int numReady = octaves[0].fifo.getNumReady();

        // --- THE LATENCY KILLER (FRAME DROPPING) ---
        // If the DSP math falls behind real-time, the FIFO backs up and creates massive visual latency.
        // If we have more than a few blocks waiting, instantly flush the old ones to catch up to live audio.
        if (numReady > 4096) {
            int samplesToDrop = numReady - 1024;
            samplesToDrop = (samplesToDrop / 1024) * 1024; // Round down to perfect 1024 chunks

            for (int oct = 0; oct < PitchengaAudioProcessor::numOctaves; ++oct) {
                int dropForOctave = samplesToDrop >> oct; // Accurately drop decimated amounts
                int start1, size1, start2, size2;
                octaves[static_cast<size_t>(oct)].fifo.prepareToRead(dropForOctave, start1, size1, start2, size2);
                octaves[static_cast<size_t>(oct)].fifo.finishedRead(size1 + size2);
            }
            numReady = octaves[0].fifo.getNumReady(); // Recalculate what's left
        }

        // Process exactly ONE 1024-sample block per wake-up
        if (numReady >= 1024) {
            for (int oct = 0; oct < PitchengaAudioProcessor::numOctaves; ++oct) {
                auto& fifo = octaves[static_cast<size_t>(oct)].fifo;
                auto& buffer = octaves[static_cast<size_t>(oct)].buffer;
                auto& win = slidingWindows[static_cast<size_t>(oct)];

                // Calculate exactly how many samples this octave should process
                // Oct 0: 1024, Oct 1: 512, Oct 2: 256...
                int samplesWanted = 1024 >> oct;
                int start1, size1, start2, size2;
                fifo.prepareToRead(samplesWanted, start1, size1, start2, size2);
                int actualRead = size1 + size2;

                if (actualRead > 0) {
                    // Shift the sliding window left
                    std::memmove(
                        win.data(),
                        win.data() + actualRead,
                        static_cast<size_t>(signalBlockSize - actualRead) * sizeof(float)
                    );

                    // Append the cleanly extracted data
                    if (size1 > 0)
                        std::copy(
                            buffer.begin() + start1,
                            buffer.begin() + (start1 + size1),
                            win.begin() + (static_cast<ptrdiff_t>(signalBlockSize) - actualRead)
                        );
                    if (size2 > 0)
                        std::copy(
                            buffer.begin() + start2,
                            buffer.begin() + (start2 + size2),
                            win.begin() + (static_cast<ptrdiff_t>(signalBlockSize) - actualRead) + size1
                        );

                    fifo.finishedRead(actualRead);
                }
            }

            // --- CQT & Smoothers ---
            for (int oct = 0; oct < PitchengaAudioProcessor::numOctaves; ++oct) {
                const auto& win = slidingWindows[static_cast<size_t>(oct)];
                if (win.size() == static_cast<size_t>(signalBlockSize)) {
                    cqt.transform(win, cqtSpectrum);

                    // --- THE THRESHOLD FIX ---
                    // Adjust this gain until your attacks feel instantly snappy again.
                    const double inputGain = 8.0;

                    int startIndex = (PitchengaAudioProcessor::numOctaves - 1 - oct) * binsPerOctave;
                    for (size_t i = 0; i < cqtSpectrum.size(); ++i) {
                        double amplitude = std::abs(cqtSpectrum[i]) * inputGain;
                        amplitudeSpectrumDb[static_cast<size_t>(startIndex) + i] = amplitudeToDbRescaled(amplitude);
                    }
                }
            }

            const auto& filteredSpectrum = allBinSmoother->smooth(amplitudeSpectrumDb);
            const auto& detectedPitchClasses = pcDetector->detectPitchClasses(filteredSpectrum);
            const auto& equalizedPitchClasses = spectralEqualizer->filter(detectedPitchClasses);

            std::fill(octaveBins.begin(), octaveBins.end(), 0.0);
            for (int i = 0; i < binsPerOctave; ++i) {
                double maxVal = 0.0;
                for (int j = i; j < static_cast<int>(equalizedPitchClasses.size()); j += binsPerOctave) {
                    maxVal = std::max(maxVal, equalizedPitchClasses[static_cast<size_t>(j)]);
                }
                octaveBins[static_cast<size_t>(i)] = maxVal;
            }

            const auto& smoothed = octaveBinSmoother->smooth(octaveBins);

            {
                const juce::CriticalSection::ScopedLockType lock(resultLock);
                if (results.size() == smoothed.size()) {
                    std::copy(smoothed.begin(), smoothed.end(), results.begin());
                    newDataAvailable.store(true);
                }
            }
        }

        wait(1);
    }
}

void PitchengaAudioProcessorEditor::CqtWorkerThread::getLatestResults(std::vector<double>& dest) {
    const juce::CriticalSection::ScopedLockType lock(resultLock);
    if (dest.size() != results.size()) dest.resize(results.size());
    std::copy(results.begin(), results.end(), dest.begin());
}

// --- Editor Implementation ---

PitchengaAudioProcessorEditor::PitchengaAudioProcessorEditor(PitchengaAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), worker(p) {
    // Add components to the UI
    addAndMakeVisible(lineTuner);
    addAndMakeVisible(circleVisualizer);

    resultsBuffer.resize(CircleVisualizer::totalFoldedBins, 0.0);

    setResizable(true, true);
    setSize(audioProcessor.lastUIWidth, audioProcessor.lastUIHeight);
    worker.startThread(juce::Thread::Priority::high);
    startTimerHz(48);
}

PitchengaAudioProcessorEditor::~PitchengaAudioProcessorEditor() {
    worker.stopThread(2000);
    stopTimer();
}

void PitchengaAudioProcessorEditor::timerCallback() {
    // --- 1. Update the Tuner ---
    // Read the lock-free atomic variable
    float latestHz = audioProcessor.currentPitchHz.load(std::memory_order_relaxed);

    // Feed it to the visualizer component
    lineTuner.setPitchFrequency(latestHz);

    // --- 2. Update the Circular Visualizer ---
    if (worker.hasNewData()) {
        worker.getLatestResults(resultsBuffer);
        worker.clearNewDataFlag();
        circleVisualizer.updateResults(resultsBuffer);
    }
}

void PitchengaAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);
}

void PitchengaAudioProcessorEditor::resized() {
    audioProcessor.lastUIWidth = getWidth();
    audioProcessor.lastUIHeight = getHeight();

    auto bounds = getLocalBounds();
    lineTuner.setBounds(bounds.removeFromBottom(static_cast<int>(LineTuner::getPreferredHeight())));
    circleVisualizer.setBounds(bounds);
}
