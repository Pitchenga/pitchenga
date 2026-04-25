#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include "../PluginProcessor.h"
#include "Cqt.h"
#include "Analyzers.h"
#include "PitchDetector.h"
#include "Stft.h"

class Math : public juce::Thread {
public:
    explicit Math(PitchengaAudioProcessor& proc);
    ~Math() override;

    void run() override;

    // Thread-safe data getters for the UI
    void getEyeResults(std::vector<double>& destinationArray);
    void getRollCqtResults(std::vector<double>& destinationArray);
    void getRollStftResults(std::vector<SpectralPeak>& destinationArray);

    bool hasNewData() const { return newDataAvailable.load(std::memory_order_acquire); }
    void clearNewDataFlag() { newDataAvailable.store(false, std::memory_order_release); }

private:
    static constexpr double inputGain = 6.0;
    
    void setupBuffers();
    void setupCqtEngine();
    void setupCqtBuffers();
    void setupPitchDetection();
    void setupStft();

    void updateSampleRate(double newSampleRate);
    static double amplitudeToDbRescaled(double amplitude);

    void flushStaleAudioData(int& availableSamples);
    void consumeAudioFromFifo();
    void processPitchDetection();
    void processStft();
    void processCqtAndEqualization();
    void publishResultsToUi();

    PitchengaAudioProcessor& processor;

    // CQT Engine for Eye and RollCqt
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

    // Pitch Engine for Needle
    std::unique_ptr<sevagh::PitchDetector<float>> pitchDetector;
    std::vector<float> rawAudioHistoryBuffer;
    std::vector<float> pitchAnalysisBuffer;

    // Decimation for Pitch Detection
    juce::dsp::IIR::Filter<float> pitchLowpass;
    std::vector<float> decimatedHistoryBuffer;
    std::vector<float> decimatedAnalysisBuffer;
    int decimationCounter = 0;

    std::vector<double> currentEyeData;
    std::vector<double> currentRollData;

    // Thread-Safe Output Buffers
    juce::CriticalSection resultLock;
    std::vector<double> eyeResults;
    std::vector<double> rollResults;
    std::atomic<bool> newDataAvailable{false};


    // STFT Engine for RollStft
    Stft stft;
    std::vector<SpectralPeak> currentRollPeaks;
    std::vector<SpectralPeak> uiRollPeaks;
};
