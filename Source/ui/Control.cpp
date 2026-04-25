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

    setupToggleButton(toggleRoll, processor.settings.isShowRoll);
    toggleRoll.onClick = [this] {
        processor.settings.isShowRoll = toggleRoll.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
        updateButtonStates();
    };

    setupToggleButton(toggleEye, processor.settings.isShowEye);
    toggleEye.onClick = [this] {
        processor.settings.isShowEye = toggleEye.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleNeedle, processor.settings.isShowNeedle);
    toggleNeedle.onClick = [this] {
        processor.settings.isShowNeedle = toggleNeedle.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleSteam, processor.settings.isShowSteam);
    toggleSteam.onClick = [this] {
        processor.settings.isShowSteam = toggleSteam.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleisFreezeRoll, processor.settings.isFreezeRoll);
    toggleisFreezeRoll.onClick = [this] {
        processor.settings.isFreezeRoll = toggleisFreezeRoll.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    toggleRollType.setButtonText(processor.settings.isUseRollStft ? "STFT" : "CQT");
    toggleRollType.onClick = [this] {
        processor.settings.isUseRollStft = !processor.settings.isUseRollStft;
        toggleRollType.setButtonText(processor.settings.isUseRollStft ? "STFT" : "CQT");
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleForrest, processor.settings.isShowForrest);
    toggleForrest.onClick = [this] {
        processor.settings.isShowForrest = toggleForrest.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleEar, processor.settings.isEarEnabled);
    toggleEar.onClick = [this] {
        processor.settings.isEarEnabled = toggleEar.getToggleState();
    };

    buttonPlugs.setButtonText("Plugs");
    buttonPlugs.onClick = [this] {
        // We use a local lambda to show the menu so we can re-invoke it for "Rescan"
        std::function<void()> showPlugsMenu = [this, &showPlugsMenu]() {
            juce::PopupMenu menu;
            menu.addItem(1, "Open Plugin Browser...");
            menu.addItem(2, "Rescan Plugins");
            menu.addSeparator();

            auto& list = processor.getKnownPluginList();
            auto types = list.getTypes();
            
            int id = 3;
            for (int i = 0; i < types.size(); ++i) {
                if (types[i].isInstrument) {
                    menu.addItem(id, types[i].name);
                }
                id++;
            }

            menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&buttonPlugs),
                [this, showPlugsMenu](int result) {
                    if (result == 1) {
                        processor.openPluginBrowser();
                    } else if (result == 2) {
                        processor.rescanPlugins();
                        // Re-show the menu after initiating the scan as requested
                        showPlugsMenu();
                    } else if (result > 2) {
                        auto& listRef = processor.getKnownPluginList();
                        auto typesRef = listRef.getTypes();
                        const int index = result - 3;
                        if (index >= 0 && index < typesRef.size()) {
                            processor.loadExternalPlugin(typesRef[static_cast<size_t>(index)]);
                        }
                    }
                });
        };
        showPlugsMenu();
    };

    buttonPlug.setButtonText("Plug");
    buttonPlug.onClick = [this] {
        processor.showExternalPluginEditor();
    };

    setupToggleButton(toggleTweak, processor.settings.isShowTweakPanel);
    toggleTweak.onClick = [this] {
        processor.settings.isShowTweakPanel = toggleTweak.getToggleState();
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
    addAndMakeVisible(toggleisFreezeRoll);
    addAndMakeVisible(toggleRollType);
    addAndMakeVisible(toggleSteam);
    addAndMakeVisible(toggleForrest);

    addAndMakeVisible(toggleEar);
    
    addAndMakeVisible(toggleTweak);
    addAndMakeVisible(tweakPanel);
    tweakPanel.addAndMakeVisible(buttonPlugs);
    tweakPanel.addAndMakeVisible(buttonPlug);
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
    toggleRoll.setToggleState(processor.settings.isShowRoll, juce::NotificationType::dontSendNotification);
    toggleEye.setToggleState(processor.settings.isShowEye, juce::NotificationType::dontSendNotification);
    toggleNeedle.setToggleState(processor.settings.isShowNeedle, juce::NotificationType::dontSendNotification);
    toggleisFreezeRoll.setToggleState(processor.settings.isFreezeRoll, juce::NotificationType::dontSendNotification);
    
    toggleRollType.setButtonText(processor.settings.isUseRollStft ? "STFT" : "CQT");
    toggleSteam.setToggleState(processor.settings.isShowSteam, juce::NotificationType::dontSendNotification);
    toggleForrest.setToggleState(processor.settings.isShowForrest, juce::NotificationType::dontSendNotification);

    toggleEar.setToggleState(processor.settings.isEarEnabled, juce::NotificationType::dontSendNotification);
    toggleTweak.setToggleState(processor.settings.isShowTweakPanel, juce::NotificationType::dontSendNotification);

    updateButtonStates();
}

void Control::updateButtonStates() {
    const bool rollActive = processor.settings.isShowRoll;
    toggleisFreezeRoll.setVisible(rollActive);
    toggleRollType.setVisible(rollActive);
    toggleSteam.setVisible(rollActive);
    toggleForrest.setVisible(rollActive);
    
    buttonPlug.setEnabled(processor.isExternalPluginLoaded());
    
    tweakPanel.setVisible(processor.settings.isShowTweakPanel);
    resized();
}

float Control::getPreferredHeight() const {
    const juce::Font font = juce::FontOptions(15.0f).withStyle("Bold");
    float h = font.getHeight() + 8.0f;
    if (processor.settings.isShowTweakPanel) {
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
    auto positionButton = [&](juce::TextButton& btn, juce::Rectangle<int>& container) {
        if (!btn.isVisible()) return;
        const float textWidth = juce::GlyphArrangement::getStringWidth(font, btn.getButtonText());
        const int buttonWidth = static_cast<int>(std::ceil(textWidth)) + 16; // 16px horizontal padding
        btn.setBounds(container.removeFromLeft(buttonWidth).reduced(2));
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

    positionButton(toggleNeedle, topRow);
    positionButton(toggleEye, topRow);
    positionButton(toggleRoll, topRow);

    positionButton(toggleEar, topRow);

    positionButtonRight(toggleTweak, topRow);

    positionButtonRight(toggleForrest, topRow);
    positionButtonRight(toggleSteam, topRow);
    positionButtonRight(toggleRollType, topRow);
    positionButtonRight(toggleisFreezeRoll, topRow);

    topRow.removeFromRight(8);
    buildTimestampLabel.setBounds(topRow);

    if (processor.settings.isShowTweakPanel) {
        tweakPanel.setBounds(bounds); // Use the entire remaining bounds (the bottom row)
        
        auto panelBounds = tweakPanel.getLocalBounds();
        positionButton(buttonPlugs, panelBounds);
        positionButton(buttonPlug, panelBounds);
        
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

    xml.setAttribute("isShowRoll", isShowRoll);
    xml.setAttribute("isShowEye", isShowEye);
    xml.setAttribute("isShowNeedle", isShowNeedle);

    xml.setAttribute("isUseRollStft", isUseRollStft);
    xml.setAttribute("isFreezeRoll", isFreezeRoll);
    xml.setAttribute("isShowForrest", isShowForrest);
    xml.setAttribute("isShowSteam", isShowSteam);

    xml.setAttribute("isEarEnabled", isEarEnabled);
    xml.setAttribute("isShowTweakPanel", isShowTweakPanel);

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

    isShowRoll = xml.getBoolAttribute("isShowRoll", isShowRoll);
    isShowEye = xml.getBoolAttribute("isShowEye", isShowEye);
    isShowNeedle = xml.getBoolAttribute("isShowNeedle", isShowNeedle);

    isUseRollStft = xml.getBoolAttribute("isUseRollStft", isUseRollStft);
    isFreezeRoll = xml.getBoolAttribute("isFreezeRoll", isFreezeRoll);
    isShowForrest = xml.getBoolAttribute("isShowForrest", isShowForrest);
    isShowSteam = xml.getBoolAttribute("isShowSteam", isShowSteam);

    isEarEnabled = xml.getBoolAttribute("isEarEnabled", isEarEnabled);
    isShowTweakPanel = xml.getBoolAttribute("isShowTweakPanel", isShowTweakPanel);

    splitRatio = static_cast<float>(xml.getDoubleAttribute("splitRatio", splitRatio));

    return true;
}
