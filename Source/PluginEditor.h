#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "Math.h"
#include "ui/Tuna.h"
#include "ui/Eye.h"
#include "ui/Roll.h"
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

    Tuna tuna;
    Eye eye;
    Roll roll;
    Splitter splitter;

    Control control;

    // Shared results for rendering
    std::vector<double> circleBuffer;
    std::vector<SpectralPeak> rollPeaksBuffer;

    static constexpr int uiRefreshRateHz = 48;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchengaAudioProcessorEditor)
};