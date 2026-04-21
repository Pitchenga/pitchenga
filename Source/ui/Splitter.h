#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>
#include "../PluginProcessor.h"

// Custom draggable 4px horizontal split bar
class Splitter : public juce::Component {
public:
    explicit Splitter(PitchengaAudioProcessor& processorToUse);
    void mouseDrag(const juce::MouseEvent& e) override;
    void paint(juce::Graphics& g) override;
    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;

    std::function<void()> onDragged;

private:
    PitchengaAudioProcessor& audioProcessor;
    bool isHovered = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Splitter)
};