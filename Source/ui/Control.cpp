#include "Control.h"
#include "../PluginProcessor.h"

Control::Control(PitchengaAudioProcessor& processorToUse)
    : audioProcessor(processorToUse) {

    setupToggleButton(toggleRoll, audioProcessor.settings.showRoll);
    toggleRoll.onClick = [this] {
        audioProcessor.settings.showRoll = toggleRoll.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
        updateButtonStates();
    };

    setupToggleButton(toggleEye, audioProcessor.settings.showEye);
    toggleEye.onClick = [this] {
        audioProcessor.settings.showEye = toggleEye.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleTuna, audioProcessor.settings.showTuna);
    toggleTuna.onClick = [this] {
        audioProcessor.settings.showTuna = toggleTuna.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleSteam, audioProcessor.settings.showSteam);
    toggleSteam.onClick = [this] {
        audioProcessor.settings.showSteam = toggleSteam.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleFreezeRoll, audioProcessor.settings.freezeRoll);
    toggleFreezeRoll.onClick = [this] {
        audioProcessor.settings.freezeRoll = toggleFreezeRoll.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleRollType, audioProcessor.settings.useStftRoll);
    toggleRollType.setButtonText(audioProcessor.settings.useStftRoll ? "STFT" : "CQT");
    toggleRollType.onClick = [this] {
        audioProcessor.settings.useStftRoll = toggleRollType.getToggleState();
        toggleRollType.setButtonText(audioProcessor.settings.useStftRoll ? "STFT" : "CQT");
        if (onVisibilityChanged) onVisibilityChanged();
        // Force resize to adjust button width based on text
        resized();
    };

    setupToggleButton(toggleForrest, audioProcessor.settings.showForrest);
    toggleForrest.onClick = [this] {
        audioProcessor.settings.showForrest = toggleForrest.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };


    addAndMakeVisible(toggleTuna);
    addAndMakeVisible(toggleEye);
    addAndMakeVisible(toggleRoll);
    addAndMakeVisible(toggleFreezeRoll);
    addAndMakeVisible(toggleRollType);
    addAndMakeVisible(toggleSteam);
    addAndMakeVisible(toggleForrest);

#include "build_timestamp.h"

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
    toggleRoll.setToggleState(audioProcessor.settings.showRoll, juce::NotificationType::dontSendNotification);
    toggleEye.setToggleState(audioProcessor.settings.showEye, juce::NotificationType::dontSendNotification);
    toggleTuna.setToggleState(audioProcessor.settings.showTuna, juce::NotificationType::dontSendNotification);
    toggleFreezeRoll.setToggleState(audioProcessor.settings.freezeRoll, juce::NotificationType::dontSendNotification);
    toggleRollType.setToggleState(audioProcessor.settings.useStftRoll, juce::NotificationType::dontSendNotification);
    toggleRollType.setButtonText(audioProcessor.settings.useStftRoll ? "STFT" : "CQT");
    toggleSteam.setToggleState(audioProcessor.settings.showSteam, juce::NotificationType::dontSendNotification);
    toggleForrest.setToggleState(audioProcessor.settings.showForrest, juce::NotificationType::dontSendNotification);

    updateButtonStates();
}

void Control::updateButtonStates() {
    const bool rollActive = audioProcessor.settings.showRoll;
    toggleFreezeRoll.setVisible(rollActive);
    toggleRollType.setVisible(rollActive);
    toggleSteam.setVisible(rollActive);
    toggleForrest.setVisible(rollActive);
    resized();
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

    auto positionButtonRight = [&](juce::TextButton& button) {
        if (!button.isVisible()) return;
        const float textWidth = juce::GlyphArrangement::getStringWidth(font, button.getButtonText());
        const int buttonWidth = static_cast<int>(std::ceil(textWidth)) + 16; // 16px horizontal padding
        button.setBounds(bounds.removeFromRight(buttonWidth).reduced(2));
    };

    positionButton(toggleTuna);
    positionButton(toggleEye);
    positionButton(toggleRoll);

    positionButtonRight(toggleForrest);
    positionButtonRight(toggleSteam);
    positionButtonRight(toggleFreezeRoll);
    positionButtonRight(toggleRollType);

    bounds.removeFromRight(8);
    buildTimestampLabel.setBounds(bounds);
}

// --- Settings Persistence ---
#define PITCHENGA_MACRO_STRING2(x) #x
#define PITCHENGA_MACRO_STRING(x) PITCHENGA_MACRO_STRING2(x)

static juce::String getSettingsTagName() {
#ifdef CMAKE_BUILD_PROFILE
    juce::String profile = PITCHENGA_MACRO_STRING(CMAKE_BUILD_PROFILE);
    profile = profile.removeCharacters("\"");
    if (profile.isEmpty()) profile = "DEFAULT";
    return "PITCHENGA_" + profile.toUpperCase();
#else
    return "PITCHENGA_DEFAULT";
#endif
}

juce::XmlElement Control::Settings::createXml() const {
    // Create an XML element to store settings
    juce::XmlElement xml(getSettingsTagName());

    // Add width and height as attributes
    xml.setAttribute("uiWidth", lastUIWidth);
    xml.setAttribute("uiHeight", lastUIHeight);

    xml.setAttribute("showRoll", showRoll);
    xml.setAttribute("showEye", showEye);
    xml.setAttribute("showTuna", showTuna);

    xml.setAttribute("useStftRoll", useStftRoll);
    xml.setAttribute("freezeRoll", freezeRoll);
    xml.setAttribute("showForrest", showForrest);
    xml.setAttribute("showSteam", showSteam);

    xml.setAttribute("splitRatio", splitRatio);

    return xml;
}

bool Control::Settings::loadFromXml(const juce::XmlElement& xml) {
    // Only if the tag matches our expected name
    if (!xml.hasTagName(getSettingsTagName())) return false;

    // Update the processor variables.
    // If the attributes don't exist, it keeps the defaults initialized in .h
    lastUIWidth = xml.getIntAttribute("uiWidth", lastUIWidth);
    lastUIHeight = xml.getIntAttribute("uiHeight", lastUIHeight);

    showRoll = xml.getBoolAttribute("showRoll", showRoll);
    showEye = xml.getBoolAttribute("showEye", showEye);
    showTuna = xml.getBoolAttribute("showTuna", showTuna);

    useStftRoll = xml.getBoolAttribute("useStftRoll", useStftRoll);
    freezeRoll = xml.getBoolAttribute("freezeRoll", freezeRoll);
    showForrest = xml.getBoolAttribute("showForrest", showForrest);
    showSteam = xml.getBoolAttribute("showSteam", showSteam);

    splitRatio = static_cast<float>(xml.getDoubleAttribute("splitRatio", splitRatio));

    return true;
}