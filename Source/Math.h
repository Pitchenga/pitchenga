#pragma once

#include <juce_core/juce_core.h>
#include "PluginProcessor.h"
#include "math/Cqt.h"
#include "Analyzers.h"
#include <pitch_detector/pitch_detector.h>
#include "math/Stft.h"

class Math : public juce::Thread {
public:
    explicit Math(PitchengaAudioProcessor& processorToUse);
    ~Math() override;

    void run() override;
    
    // Thread-safe data getters for the UI
    void getCircleResults(std::vector<double>& destinationArray);
    void getLineResults(std::vector<double>& destinationArray);
    
    bool hasNewData() const { return newDataAvailable.load(std::memory_order_acquire); }
    void clearNewDataFlag() { newDataAvailable.store(false, std::memory_order_release); }

    const Cqt* getCqtEngine() const { return &cqtEngine; }
    void getRollPeaks(std::vector<SpectralPeak>& destinationArray);

private:
    static constexpr double inputGain = 6.0;
    
    void setupBuffers();
    void setupCqtEngine();
    void setupCqtBuffers();
    void setupPitchDetection();

    void updateSampleRate(double newSampleRate);
    static double amplitudeToDbRescaled(double amplitude);

    void flushStaleAudioData(int& availableSamples) const;
    void consumeAudioFromFifo();
    void processPitchDetection();
    void processCqtAndEqualization();
    void publishResultsToUi();

    PitchengaAudioProcessor& audioProcessor;

    // --- CQT Engine for the Eye ---
    Cqt cqtEngine;
    std::unique_ptr<HarmonicPatternPitchClassDetector> pitchClassDetector;
    std::unique_ptr<SpectralEqualizer> spectralEqualizer;
    std::unique_ptr<ExpSmoother> octaveBinSmoother;
    std::unique_ptr<ExpSmoother> allBinSmoother;

    std::vector<float> workingBuffer;
    std::vector<std::vector<float>> slidingWindows;
    std::vector<std::complex<float>> cqtSpectrum;
    std::vector<double> amplitudeSpectrumDb;
    std::vector<double> octaveBins;

    // --- Pitch Engine for the Tuna ---
    std::unique_ptr<adamski::PitchMPM> pitchDetector;
    std::vector<float> rawAudioHistoryBuffer;
    std::vector<float> pitchAnalysisBuffer;

    std::vector<double> currentEyeData;
    std::vector<double> currentRollData;

    // --- Thread-Safe Output Buffers ---
    juce::CriticalSection resultLock;
    std::vector<double> circleVisualizerResults;
    std::vector<double> lineVisualizerResults;
    std::atomic<bool> newDataAvailable{false};


    // -- STFT Engine for StftRoll
    Stft stft;
    std::vector<SpectralPeak> currentRollPeaks;
    std::vector<SpectralPeak> uiRollPeaks;
    void setupStft();
    void processStft();
};