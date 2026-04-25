#include "Control.h"
#include "../PluginProcessor.h"

// --- Settings Persistence Helpers ---
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

Control::Control(PitchengaAudioProcessor& proc)
    : processor(proc) {

    setupToggleButton(toggleRoll, processor.settings.showRoll);
    toggleRoll.onClick = [this] {
        processor.settings.showRoll = toggleRoll.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
        updateButtonStates();
    };

    setupToggleButton(toggleEye, processor.settings.showEye);
    toggleEye.onClick = [this] {
        processor.settings.showEye = toggleEye.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleNeedle, processor.settings.showNeedle);
    toggleNeedle.onClick = [this] {
        processor.settings.showNeedle = toggleNeedle.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleSteam, processor.settings.showSteam);
    toggleSteam.onClick = [this] {
        processor.settings.showSteam = toggleSteam.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleFreezeRoll, processor.settings.freezeRoll);
    toggleFreezeRoll.onClick = [this] {
        processor.settings.freezeRoll = toggleFreezeRoll.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    toggleRollType.setButtonText(processor.settings.useStftRoll ? "STFT" : "CQT");
    toggleRollType.onClick = [this] {
        processor.settings.useStftRoll = !processor.settings.useStftRoll;
        toggleRollType.setButtonText(processor.settings.useStftRoll ? "STFT" : "CQT");
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleForrest, processor.settings.showForrest);
    toggleForrest.onClick = [this] {
        processor.settings.showForrest = toggleForrest.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleTweak, showTweakPanel);
    toggleTweak.onClick = [this] {
        showTweakPanel = toggleTweak.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
        updateButtonStates();
    };

    buttonCopy.setButtonText("Copy");
    buttonCopy.setColour(juce::TextButton::buttonColourId, juce::Colours::black.withAlpha(0.4f));
    buttonCopy.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    buttonCopy.onClick = [this] {
        juce::XmlElement xml = processor.settings.createXml();
        juce::SystemClipboard::copyTextToClipboard(xml.toString());
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon, "Copied", "Settings copied to clipboard.");
    };

    buttonNuke.setColour(juce::TextButton::buttonColourId, juce::Colours::darkred.withAlpha(0.6f));
    buttonNuke.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    buttonNuke.onClick = [this] {
        juce::AlertWindow::showAsync(
            juce::MessageBoxOptions()
                .withIconType(juce::MessageBoxIconType::WarningIcon)
                .withTitle("Reset Settings")
                .withMessage("Are you sure you want to reset to factory settings?")
                .withButton("Yes")
                .withButton("No")
                .withAssociatedComponent(this),
            [this](int result) {
                if (result == 1) { // 1 is the index of the first button ("Yes")
                    juce::File defaultSettingsFile(juce::File(__FILE__).getParentDirectory().getParentDirectory().getChildFile("settings-default.xml"));
                    if (auto xml = juce::XmlDocument::parse(defaultSettingsFile)) {
                        xml->setTagName(getSettingsTagName()); // Ensure the tag matches what loadFromXml expects
                        bool loaded = processor.settings.loadFromXml(*xml);
                        if (loaded) {
                            updateVisibilityFromState();
                            if (onVisibilityChanged) onVisibilityChanged();
                        } else {
                            juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Error", "Failed to apply settings from XML.");
                        }
                    } else {
                        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, "Error", "Could not find or parse: " + defaultSettingsFile.getFullPathName());
                    }
                }
            }
        );
    };


    addAndMakeVisible(toggleNeedle);
    addAndMakeVisible(toggleEye);
    addAndMakeVisible(toggleRoll);
    addAndMakeVisible(toggleFreezeRoll);
    addAndMakeVisible(toggleRollType);
    addAndMakeVisible(toggleSteam);
    addAndMakeVisible(toggleForrest);

    addAndMakeVisible(toggleTweak);
    addAndMakeVisible(tweakPanel);
    tweakPanel.addAndMakeVisible(buttonCopy);
    tweakPanel.addAndMakeVisible(buttonNuke);

#include "build_timestamp.h"

    buildTimestampLabel.setText(juce::String("Build ") + BUILD_TIMESTAMP, juce::NotificationType::dontSendNotification);
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
    toggleRoll.setToggleState(processor.settings.showRoll, juce::NotificationType::dontSendNotification);
    toggleEye.setToggleState(processor.settings.showEye, juce::NotificationType::dontSendNotification);
    toggleNeedle.setToggleState(processor.settings.showNeedle, juce::NotificationType::dontSendNotification);
    toggleFreezeRoll.setToggleState(processor.settings.freezeRoll, juce::NotificationType::dontSendNotification);
    
    toggleRollType.setButtonText(processor.settings.useStftRoll ? "STFT" : "CQT");
    toggleSteam.setToggleState(processor.settings.showSteam, juce::NotificationType::dontSendNotification);
    toggleForrest.setToggleState(processor.settings.showForrest, juce::NotificationType::dontSendNotification);

    updateButtonStates();
}

void Control::updateButtonStates() {
    const bool rollActive = processor.settings.showRoll;
    toggleFreezeRoll.setVisible(rollActive);
    toggleRollType.setVisible(rollActive);
    toggleSteam.setVisible(rollActive);
    toggleForrest.setVisible(rollActive);
    tweakPanel.setVisible(showTweakPanel);
    resized();
}

float Control::getPreferredHeight() const {
    const juce::Font font = juce::FontOptions(15.0f).withStyle("Bold");
    float h = font.getHeight() + 8.0f;
    if (showTweakPanel) {
        h *= 2.0f;
    }
    return h;
}

void Control::resized() {
    auto bounds = getLocalBounds();

    const juce::Font font = juce::FontOptions(15.0f).withStyle("Bold");

    // Calculate row height
    int rowHeight = static_cast<int>(font.getHeight() + 8.0f);
    
    // Create the top row
    auto topRow = bounds.removeFromTop(rowHeight);

    // Pack the buttons to the left with minimal offsets dynamically sizing to their text
    auto positionButton = [&](juce::TextButton& btn) {
        const float textWidth = juce::GlyphArrangement::getStringWidth(font, btn.getButtonText());
        const int buttonWidth = static_cast<int>(std::ceil(textWidth)) + 16; // 16px horizontal padding
        btn.setBounds(topRow.removeFromLeft(buttonWidth).reduced(2));
    };

    auto positionButtonRight = [&](juce::TextButton& button, juce::Rectangle<int>& container) {
        if (!button.isVisible()) return;
        float textWidth = juce::GlyphArrangement::getStringWidth(font, button.getButtonText());
        if (&button == &toggleRollType) {
            textWidth = juce::jmax(juce::GlyphArrangement::getStringWidth(font, "STFT"),
                                   juce::GlyphArrangement::getStringWidth(font, "CQT"));
        }
        const int buttonWidth = static_cast<int>(std::ceil(textWidth)) + 16; // 16px horizontal padding
        button.setBounds(container.removeFromRight(buttonWidth).reduced(2));
    };

    positionButton(toggleNeedle);
    positionButton(toggleEye);
    positionButton(toggleRoll);

    positionButtonRight(toggleTweak, topRow);

    positionButtonRight(toggleForrest, topRow);
    positionButtonRight(toggleSteam, topRow);
    positionButtonRight(toggleRollType, topRow);
    positionButtonRight(toggleFreezeRoll, topRow);

    topRow.removeFromRight(8);
    buildTimestampLabel.setBounds(topRow);

    if (showTweakPanel) {
        tweakPanel.setBounds(bounds); // Use the entire remaining bounds (the bottom row)
        
        auto panelBounds = tweakPanel.getLocalBounds();
        positionButtonRight(buttonNuke, panelBounds);
        positionButtonRight(buttonCopy, panelBounds);
    }
}

juce::XmlElement Control::Settings::createXml() const {
    // Create an XML element to store settings
    juce::XmlElement xml(getSettingsTagName());

    // Add width and height as attributes
    xml.setAttribute("uiWidth", lastUIWidth);
    xml.setAttribute("uiHeight", lastUIHeight);

    xml.setAttribute("showRoll", showRoll);
    xml.setAttribute("showEye", showEye);
    xml.setAttribute("showNeedle", showNeedle);

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
    showNeedle = xml.getBoolAttribute("showNeedle", showNeedle);

    useStftRoll = xml.getBoolAttribute("useStftRoll", useStftRoll);
    freezeRoll = xml.getBoolAttribute("freezeRoll", freezeRoll);
    showForrest = xml.getBoolAttribute("showForrest", showForrest);
    showSteam = xml.getBoolAttribute("showSteam", showSteam);

    splitRatio = static_cast<float>(xml.getDoubleAttribute("splitRatio", splitRatio));

    return true;
}