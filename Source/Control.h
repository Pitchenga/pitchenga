#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include <functional>

class Control : public juce::Component {
public:
    explicit Control(PitchengaAudioProcessor& processorToUse);
    ~Control() override = default;

    void resized() override;
    void updateVisibilityFromState();

    // Callback so the Editor knows when a user clicked a toggle
    std::function<void()> onVisibilityChanged;

private:
    void setupToggleButton(juce::TextButton& button, bool initialState);

    PitchengaAudioProcessor& audioProcessor;

    juce::TextButton toggleLineViz{"Line"};
    juce::TextButton toggleCircleViz{"Circle"};
    juce::TextButton toggleTunerViz{"Tuner"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Control)
};