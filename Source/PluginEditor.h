#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "MathWorker.h"
#include "TunerViz.h"
#include "CircleViz.h"
#include "LineViz.h"

class PitchengaAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    explicit PitchengaAudioProcessorEditor(PitchengaAudioProcessor&);
    ~PitchengaAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void updateVisibilityFromState();

private:
    void timerCallback() override;
    void setupToggleButton(juce::TextButton& button, bool initialState);

    PitchengaAudioProcessor& audioProcessor;

    // Background worker for heavy DSP
    MathWorker worker;

    TunerViz tunerViz;
    CircleViz circleViz;
    LineViz lineViz;

    juce::TextButton toggleLineViz{"Line"};
    juce::TextButton toggleCircleViz{"Circle"};
    juce::TextButton toggleTunerViz{"Tuner"};

    // Shared results for rendering
    std::vector<double> circleBuffer;
    std::vector<double> lineBuffer;

    static constexpr int uiRefreshRateHz = 48;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchengaAudioProcessorEditor)
};