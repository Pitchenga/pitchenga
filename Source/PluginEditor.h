#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>
#include "PluginProcessor.h"
#include "CqtEngine.h"
#include "Analyzers.h"
#include "LineTuner.h"
#include "CircleVisualizer.h"
#include <array>
#include <vector>
#include <memory>
#include <atomic>


class PitchengaAudioProcessorEditor : public juce::AudioProcessorEditor,
                                      private juce::Timer
{
public:
    explicit PitchengaAudioProcessorEditor (PitchengaAudioProcessor&);
    ~PitchengaAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    PitchengaAudioProcessor& audioProcessor;

    // Background worker for heavy DSP
    class CqtWorkerThread : public juce::Thread
    {
    public:
        CqtWorkerThread (PitchengaAudioProcessor& p);
        void run() override;
        
        void getLatestResults (std::vector<double>& dest);
        bool hasNewData() const { return newDataAvailable.load(); }
        void clearNewDataFlag() { newDataAvailable.store (false); }

        void updateSampleRate (double newSampleRate);

    private:
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
        std::atomic<bool> newDataAvailable { false };
        
        static double amplitudeToDbRescaled (double amplitude);
    };

    CqtWorkerThread worker;
    LineTuner lineTuner;
    CircleVisualizer circleVisualizer;

    // Shared results for rendering
    std::vector<double> resultsBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchengaAudioProcessorEditor)
};
