#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include "PluginProcessor.h"
#include "CqtEngine.h"
#include "Analyzers.h"
#include <pitch_detector/pitch_detector.h>

class MathWorker : public juce::Thread {
public:
    explicit MathWorker(PitchengaAudioProcessor& processorToUse);
    ~MathWorker() override;

    void run() override;
    
    // Thread-safe data getters for the UI
    void getCircleResults(std::vector<double>& destinationArray);
    void getLineResults(std::vector<double>& destinationArray);
    
    bool hasNewData() const { return newDataAvailable.load(std::memory_order_acquire); }
    void clearNewDataFlag() { newDataAvailable.store(false, std::memory_order_release); }

    const CqtEngine* getCqtEngine() const { return &cqtEngine; }

private:
    static constexpr double inputGain = 6.0;
    void setupBuffers();
    void updateSampleRate(double newSampleRate);
    static double amplitudeToDbRescaled(double amplitude);

    PitchengaAudioProcessor& audioProcessor;

    // --- CQT Engine (For CircleViz) ---
    CqtEngine cqtEngine;
    std::unique_ptr<HarmonicPatternPitchClassDetector> pitchClassDetector;
    std::unique_ptr<SpectralEqualizer> spectralEqualizer;
    std::unique_ptr<ExpSmoother> octaveBinSmoother;
    std::unique_ptr<ExpSmoother> allBinSmoother;

    std::vector<float> workingBuffer;
    std::vector<std::vector<float>> slidingWindows;
    std::vector<std::complex<float>> cqtSpectrum;
    std::vector<double> amplitudeSpectrumDb;
    std::vector<double> octaveBins;

    // --- FFT Engine (For LineViz) ---
    std::unique_ptr<juce::dsp::FFT> fastFourierTransform;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> windowingFunction;
    std::vector<float> rawAudioHistoryBuffer;
    std::vector<float> complexFftWorkspace;
    std::vector<double> linearFftMagnitudes;

    // --- Pitch Engine (For TunerViz) ---
    std::unique_ptr<adamski::PitchMPM> pitchDetector;
    std::vector<float> pitchAnalysisBuffer;

    // --- Thread-Safe Output Buffers ---
    juce::CriticalSection resultLock;
    std::vector<double> circleVisualizerResults;
    std::vector<double> lineVisualizerResults;
    std::atomic<bool> newDataAvailable{false};
};