#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "math/Math.h"
#include "ui/Needle.h"
#include "ui/Eye.h"
#include "ui/RollStft.h"
#include "ui/RollCqt.h"
#include "ui/Control.h"
#include "ui/Splitter.h"

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

    Needle needle;
    Eye eye;
    RollStft stftRoll;
    RollCqt cqtRoll;
    Splitter splitter;

    Control control;

    // Shared results for rendering
    std::vector<double> circleBuffer;
    std::vector<SpectralPeak> rollPeaksBuffer;
    std::vector<double> cqtRollBuffer;

    static constexpr int uiRefreshRateHz = 144;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchengaAudioProcessorEditor)
};