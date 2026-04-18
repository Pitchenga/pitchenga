#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>
#include "PluginProcessor.h"
#include "CqtEngine.h"
#include "Analyzers.h"
#include "TunerViz.h"
#include "CircleViz.h"
#include "LineViz.h"
#include <array>
#include <vector>
#include <memory>
#include <atomic>


class PitchengaAudioProcessorEditor : public juce::AudioProcessorEditor,
    private juce::Timer {
public:
    explicit PitchengaAudioProcessorEditor(PitchengaAudioProcessor&);
    ~PitchengaAudioProcessorEditor() override;

    void timerCallback() override;
    void paint(juce::Graphics&) override;
    void resized() override;

    int uiRefreshRateHz = 48;
private:
    PitchengaAudioProcessor& audioProcessor;

    // Background worker for heavy DSP
    class CqtWorkerThread : public juce::Thread {
    public:
        CqtWorkerThread(PitchengaAudioProcessor& p);
        void run() override;

        void getLatestResults(std::vector<double>& dest);
        void getFullSpectrumResults(std::vector<double>& dest);
        const CqtEngine* getCqtEngine() const { return &cqt; }
        bool hasNewData() const { return newDataAvailable.load(); }
        void clearNewDataFlag() { newDataAvailable.store(false); }

        void updateSampleRate(double newSampleRate);

    private:
        static constexpr double inputGain = 6.0;
        void setupBuffers();

        PitchengaAudioProcessor& processor;

        CqtEngine cqt;
        std::unique_ptr<HarmonicPatternPitchClassDetector> pcDetector;
        std::unique_ptr<SpectralEqualizer> spectralEqualizer;
        std::unique_ptr<ExpSmoother> allBinSmoother;
        std::unique_ptr<ExpSmoother> octaveBinSmoother;

        std::vector<float> workBuffer;
        std::vector<std::vector<float>> slidingWindows;
        std::vector<std::complex<float>> cqtSpectrum;
        std::vector<double> amplitudeSpectrumDb;
        std::vector<double> octaveBins;

        juce::CriticalSection resultLock;
        std::vector<double> results;
        std::vector<double> fullSpectrumResults;
        std::atomic<bool> newDataAvailable{false};

        static double amplitudeToDbRescaled(double amplitude);

        std::unique_ptr<adamski::PitchMPM> pitchDetector;
        std::vector<float> pitchBuffer;
        std::vector<float> pitchAnalysisBuffer;
    };

    CqtWorkerThread worker;
    TunerViz tunerViz;
    LineViz lineViz;
    CircleViz circleViz;

    // Shared results for rendering
    std::vector<double> resultsBuffer;
    std::vector<double> fullSpectrumBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchengaAudioProcessorEditor)
};
