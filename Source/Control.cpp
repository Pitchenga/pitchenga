#include "Control.h"

Control::Control(PitchengaAudioProcessor& processorToUse)
    : audioProcessor(processorToUse) {

    setupToggleButton(toggleLineViz, audioProcessor.showLineViz);
    toggleLineViz.onClick = [this] {
        audioProcessor.showLineViz = toggleLineViz.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleCircleViz, audioProcessor.showCircleViz);
    toggleCircleViz.onClick = [this] {
        audioProcessor.showCircleViz = toggleCircleViz.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleTunerViz, audioProcessor.showTunerViz);
    toggleTunerViz.onClick = [this] {
        audioProcessor.showTunerViz = toggleTunerViz.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    addAndMakeVisible(toggleLineViz);
    addAndMakeVisible(toggleCircleViz);
    addAndMakeVisible(toggleTunerViz);
}

void Control::setupToggleButton(juce::TextButton& button, bool initialState) {
    button.setClickingTogglesState(true);
    button.setToggleState(initialState, juce::NotificationType::dontSendNotification);
    button.setColour(juce::TextButton::buttonColourId, juce::Colours::black.withAlpha(0.4f));
    button.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white.withAlpha(0.2f));
    button.setColour(juce::TextButton::textColourOffId, juce::Colours::grey);
    button.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
}

void Control::updateVisibilityFromState() {
    toggleLineViz.setToggleState(audioProcessor.showLineViz, juce::NotificationType::dontSendNotification);
    toggleCircleViz.setToggleState(audioProcessor.showCircleViz, juce::NotificationType::dontSendNotification);
    toggleTunerViz.setToggleState(audioProcessor.showTunerViz, juce::NotificationType::dontSendNotification);
}

void Control::resized() {
    auto bounds = getLocalBounds();

    // fixme: Un-hardcode sizes
    // Pack the buttons to the left with minimal offsets
    toggleLineViz.setBounds(bounds.removeFromLeft(60).reduced(2));
    toggleCircleViz.setBounds(bounds.removeFromLeft(60).reduced(2));
    toggleTunerViz.setBounds(bounds.removeFromLeft(60).reduced(2));
}