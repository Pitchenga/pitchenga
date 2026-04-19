#include "Control.h"
#include "PluginProcessor.h"

Control::Control(PitchengaAudioProcessor& processorToUse)
    : audioProcessor(processorToUse) {

    setupToggleButton(toggleRoll, audioProcessor.uiSettings.showRoll);
    toggleRoll.onClick = [this] {
        audioProcessor.uiSettings.showRoll = toggleRoll.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleEye, audioProcessor.uiSettings.showEye);
    toggleEye.onClick = [this] {
        audioProcessor.uiSettings.showEye = toggleEye.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleTunaViz, audioProcessor.uiSettings.showTunaViz);
    toggleTunaViz.onClick = [this] {
        audioProcessor.uiSettings.showTunaViz = toggleTunaViz.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleForrest, audioProcessor.uiSettings.showForrest);
    toggleForrest.onClick = [this] {
        audioProcessor.uiSettings.showForrest = toggleForrest.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleSteam, audioProcessor.uiSettings.showSteam);
    toggleSteam.onClick = [this] {
        audioProcessor.uiSettings.showSteam = toggleSteam.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    addAndMakeVisible(toggleTunaViz);
    addAndMakeVisible(toggleEye);
    addAndMakeVisible(toggleRoll);
    addAndMakeVisible(toggleSteam);
    addAndMakeVisible(toggleForrest);
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
    toggleRoll.setToggleState(audioProcessor.uiSettings.showRoll, juce::NotificationType::dontSendNotification);
    toggleEye.setToggleState(audioProcessor.uiSettings.showEye, juce::NotificationType::dontSendNotification);
    toggleTunaViz.setToggleState(audioProcessor.uiSettings.showTunaViz, juce::NotificationType::dontSendNotification);
    toggleForrest.setToggleState(audioProcessor.uiSettings.showForrest, juce::NotificationType::dontSendNotification);
    toggleSteam.setToggleState(audioProcessor.uiSettings.showSteam, juce::NotificationType::dontSendNotification);
}

void Control::resized() {
    auto bounds = getLocalBounds();

    // fixme: Un-hardcode sizes
    // Pack the buttons to the left with minimal offsets
    toggleRoll.setBounds(bounds.removeFromLeft(60).reduced(2));
    toggleEye.setBounds(bounds.removeFromLeft(60).reduced(2));
    toggleTunaViz.setBounds(bounds.removeFromLeft(60).reduced(2));
    toggleForrest.setBounds(bounds.removeFromLeft(80).reduced(2));
    toggleSteam.setBounds(bounds.removeFromLeft(95).reduced(2));
}

// --- Settings Persistence ---
juce::XmlElement Control::Settings::createXml() const {
    // Create an XML element to store settings
    juce::XmlElement xml("PITCHENGA_SETTINGS");

    // Add width and height as attributes
    xml.setAttribute("uiWidth", lastUIWidth);
    xml.setAttribute("uiHeight", lastUIHeight);

    xml.setAttribute("showRoll", showRoll);
    xml.setAttribute("showEye", showEye);
    xml.setAttribute("showTunaViz", showTunaViz);

    xml.setAttribute("showForrest", showForrest);
    xml.setAttribute("showSteam", showSteam);

    xml.setAttribute("splitRatio", static_cast<double>(splitRatio));

    return xml;
}

bool Control::Settings::loadFromXml(const juce::XmlElement& xml) {
    // Only if the tag matches our expected name
    if (!xml.hasTagName("PITCHENGA_SETTINGS")) return false;

    // Update the processor variables.
    // If the attributes don't exist, it keeps the defaults initialized in .h
    lastUIWidth = xml.getIntAttribute("uiWidth", lastUIWidth);
    lastUIHeight = xml.getIntAttribute("uiHeight", lastUIHeight);

    showRoll = xml.getBoolAttribute("showRoll", showRoll);
    showEye = xml.getBoolAttribute("showEye", showEye);
    showTunaViz = xml.getBoolAttribute("showTunaViz", showTunaViz);

    showForrest = xml.getBoolAttribute("showForrest", showForrest);
    showSteam = xml.getBoolAttribute("showSteam", showSteam);

    splitRatio = static_cast<float>(xml.getDoubleAttribute("splitRatio", splitRatio));

    return true;
}