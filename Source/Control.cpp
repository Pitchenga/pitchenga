#include "Control.h"
#include "PluginProcessor.h"

Control::Control(PitchengaAudioProcessor& processorToUse)
    : audioProcessor(processorToUse) {

    setupToggleButton(toggleRoll, audioProcessor.uiSettings.showRoll);
    toggleRoll.onClick = [this] {
        audioProcessor.uiSettings.showRoll = toggleRoll.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
        updateButtonStates();
    };

    setupToggleButton(toggleEye, audioProcessor.uiSettings.showEye);
    toggleEye.onClick = [this] {
        audioProcessor.uiSettings.showEye = toggleEye.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleTuna, audioProcessor.uiSettings.showTuna);
    toggleTuna.onClick = [this] {
        audioProcessor.uiSettings.showTuna = toggleTuna.getToggleState();
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

    addAndMakeVisible(toggleTuna);
    addAndMakeVisible(toggleEye);
    addAndMakeVisible(toggleRoll);
    addAndMakeVisible(toggleForrest);
    addAndMakeVisible(toggleSteam);

#ifndef BUILD_TIMESTAMP
#define BUILD_TIMESTAMP "Unknown"
#endif

    buildTimestampLabel.setText(juce::String("Build: ") + BUILD_TIMESTAMP, juce::NotificationType::dontSendNotification);
    buildTimestampLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    buildTimestampLabel.setFont(juce::FontOptions(13.0f));
    buildTimestampLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(buildTimestampLabel);

    updateButtonStates();
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
    toggleTuna.setToggleState(audioProcessor.uiSettings.showTuna, juce::NotificationType::dontSendNotification);
    toggleForrest.setToggleState(audioProcessor.uiSettings.showForrest, juce::NotificationType::dontSendNotification);
    toggleSteam.setToggleState(audioProcessor.uiSettings.showSteam, juce::NotificationType::dontSendNotification);

    updateButtonStates();
}

void Control::updateButtonStates() {
    const bool rollActive = audioProcessor.uiSettings.showRoll;
    toggleForrest.setEnabled(rollActive);
    toggleSteam.setEnabled(rollActive);
}

float Control::getPreferredHeight() {
    const juce::Font font = juce::FontOptions(15.0f).withStyle("Bold");
    return font.getHeight() + 8.0f;
}

void Control::resized() {
    auto bounds = getLocalBounds();

    const juce::Font font = juce::FontOptions(15.0f).withStyle("Bold");

    // Pack the buttons to the left with minimal offsets dynamically sizing to their text
    auto positionButton = [&](juce::TextButton& btn) {
        const float textWidth = juce::GlyphArrangement::getStringWidth(font, btn.getButtonText());
        const int buttonWidth = static_cast<int>(std::ceil(textWidth)) + 16; // 16px horizontal padding
        btn.setBounds(bounds.removeFromLeft(buttonWidth).reduced(2));
    };

    positionButton(toggleTuna);
    positionButton(toggleEye);
    positionButton(toggleRoll);
    positionButton(toggleForrest);
    positionButton(toggleSteam);

    bounds.removeFromLeft(8);
    buildTimestampLabel.setBounds(bounds);
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
    xml.setAttribute("showTuna", showTuna);

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
    showTuna = xml.getBoolAttribute("showTuna", showTuna);

    showForrest = xml.getBoolAttribute("showForrest", showForrest);
    showSteam = xml.getBoolAttribute("showSteam", showSteam);

    splitRatio = static_cast<float>(xml.getDoubleAttribute("splitRatio", splitRatio));

    return true;
}