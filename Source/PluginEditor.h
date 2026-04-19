#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "Math.h"
#include "Tuna.h"
#include "Eye.h"
#include "LineViz.h"
#include "Control.h"
#include "Splitter.h"

class PitchengaAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    explicit PitchengaAudioProcessorEditor(PitchengaAudioProcessor&);
    ~PitchengaAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void updateVisibilityFromState();

private:
    void timerCallback() override;

    PitchengaAudioProcessor& audioProcessor;

    // Background worker for heavy DSP
    Math worker;

    Tuna tunerViz;
    Eye circleViz;
    LineViz lineViz;
    Splitter splitterBar;

    Control controlOverlay;

    // Shared results for rendering
    std::vector<double> circleBuffer;
    std::vector<double> lineBuffer;

    static constexpr int uiRefreshRateHz = 48;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchengaAudioProcessorEditor)
};