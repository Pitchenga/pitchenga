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
            menu.addItem(1, "Open Plugs Browser...");
            menu.addItem(2, "Rescan Plugs");
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

            menu.showMenuAsync(
                juce::PopupMenu::Options().withTargetComponent(&buttonPlugs),
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
                            processor.loadExternalPlugin(typesRef[static_cast<size_t>(index)], true);
                        }
                    }
                }
            );
        };
        showPlugsMenu();
    };

    buttonPlug.setButtonText("Plug");
    buttonPlug.onClick = [this] {
        processor.showExternalPluginEditor();
    };

    comboPresets.setTextWhenNothingSelected("Presets...");
    comboPresets.onChange = [this] {
        const int id = comboPresets.getSelectedId();
        if (id == 1) {
            // Load factory settings-default.xml
            const juce::File factoryDefaultFile(juce::File(__FILE__).getParentDirectory().getParentDirectory().getChildFile("settings-default.xml"));
            if (auto xml = juce::XmlDocument::parse(factoryDefaultFile)) {
                xml->setTagName(getSettingsTagName());
                if (processor.settings.loadFromXml(*xml)) {
                    updateVisibilityFromState();
                    if (onVisibilityChanged) onVisibilityChanged();
                    
                    juce::MemoryBlock destData;
                    PitchengaAudioProcessor::copyXmlToBinary(*xml, destData);
                    processor.setStateInformation(destData.getData(), static_cast<int>(destData.getSize()));
                    
                    // Factory is read-only
                    currentPresetFile = juce::File();
                    comboPresets.setText("Factory Default", juce::NotificationType::dontSendNotification);
                }
            }
        } else if (id == 2) {
            // Load user-default.xml
            const auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("Pitchenga");
            const auto userDefaultFile = appDataDir.getChildFile("user-default.xml");
            
            if (userDefaultFile.existsAsFile()) {
                if (auto xml = juce::XmlDocument::parse(userDefaultFile)) {
                    if (processor.settings.loadFromXml(*xml)) {
                        updateVisibilityFromState();
                        if (onVisibilityChanged) onVisibilityChanged();
                        
                        juce::MemoryBlock destData;
                        PitchengaAudioProcessor::copyXmlToBinary(*xml, destData);
                        processor.setStateInformation(destData.getData(), static_cast<int>(destData.getSize()));
                        currentPresetFile = userDefaultFile;
                        comboPresets.setText("User Default", juce::NotificationType::dontSendNotification);
                    }
                }
            } else {
                // Fallback to factory if missing, but keep User Default context for Saving
                const juce::File factoryDefaultFile(juce::File(__FILE__).getParentDirectory().getParentDirectory().getChildFile("settings-default.xml"));
                if (auto xml = juce::XmlDocument::parse(factoryDefaultFile)) {
                    xml->setTagName(getSettingsTagName());
                    if (processor.settings.loadFromXml(*xml)) {
                        updateVisibilityFromState();
                        if (onVisibilityChanged) onVisibilityChanged();
                        
                        juce::MemoryBlock destData;
                        PitchengaAudioProcessor::copyXmlToBinary(*xml, destData);
                        processor.setStateInformation(destData.getData(), static_cast<int>(destData.getSize()));
                        
                        currentPresetFile = userDefaultFile;
                        comboPresets.setText("User Default", juce::NotificationType::dontSendNotification);
                    }
                }
            }
        } else if (id == 3) {
            // File... (Load file dialog)
            chooser = std::make_unique<juce::FileChooser>(
                "Select a settings file to load...",
                juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("Pitchenga"),
                "*.xml"
            );

            auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

            chooser->launchAsync(
                flags,
                [this](const juce::FileChooser& fc) {
                    auto result = fc.getResult();
                    if (result.existsAsFile()) {
                        if (auto xml = juce::XmlDocument::parse(result)) {
                            if (processor.settings.loadFromXml(*xml)) {
                                updateVisibilityFromState();
                                if (onVisibilityChanged) onVisibilityChanged();

                                juce::MemoryBlock destData;
                                PitchengaAudioProcessor::copyXmlToBinary(*xml, destData);
                                processor.setStateInformation(destData.getData(), static_cast<int>(destData.getSize()));
                                currentPresetFile = result;
                                refreshPresets(); // To handle case where file was outside the dir
                            }
                        }
                    }
                }
            );
        } else if (id > 3) {
            const size_t index = static_cast<size_t>(id - 4);
            if (index < presets.size()) {
                const auto& presetFile = presets[index];
                if (auto xml = juce::XmlDocument::parse(presetFile)) {
                    if (processor.settings.loadFromXml(*xml)) {
                        updateVisibilityFromState();
                        if (onVisibilityChanged) onVisibilityChanged();
                        
                        juce::MemoryBlock destData;
                        PitchengaAudioProcessor::copyXmlToBinary(*xml, destData);
                        processor.setStateInformation(destData.getData(), static_cast<int>(destData.getSize()));
                        currentPresetFile = presetFile;
                        comboPresets.setText(presetFile.getFileNameWithoutExtension(), juce::NotificationType::dontSendNotification);
                    }
                }
            }
        }
        updateButtonStates();
    };
    refreshPresets();

    setupToggleButton(toggleTweak, processor.settings.isShowTweakPanel);
    toggleTweak.onClick = [this] {
        processor.settings.isShowTweakPanel = toggleTweak.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
        updateButtonStates();
    };

    buttonSave.setButtonText("Save");
    buttonSave.onClick = [this] {
        if (currentPresetFile == juce::File()) {
            return;
        }

        // Sync current plugin state into settings first
        juce::MemoryBlock dummy;
        processor.getStateInformation(dummy);
        
        const juce::XmlElement xml = processor.settings.createXml();
        if (xml.writeTo(currentPresetFile)) {
            refreshPresets();
            // Explicitly force text update for transitions like Factory -> User Default
            if (currentPresetFile.getFileName() == "user-default.xml") {
                comboPresets.setText("User Default", juce::NotificationType::dontSendNotification);
            } else {
                comboPresets.setText(currentPresetFile.getFileNameWithoutExtension(), juce::NotificationType::dontSendNotification);
            }
        } else {
            juce::AlertWindow::showMessageBoxAsync(
                juce::MessageBoxIconType::WarningIcon,
                "Error",
                "Failed to overwrite setup file."
            );
        }
    };

    buttonSaveAs.setButtonText("Save As");
    buttonSaveAs.onClick = [this] {
        const juce::String currentName = comboPresets.getText();
        juce::String suggestedName = currentName;
        
        if (currentName == "Presets..." || currentName == "Factory Default" || currentName == "User Default") {
            suggestedName = "user-default";
        }

        chooser = std::make_unique<juce::FileChooser>(
            "Select where to save the settings...",
            juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("Pitchenga").getChildFile(suggestedName),
            "*.xml"
        );

        auto flags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles |
            juce::FileBrowserComponent::warnAboutOverwriting;

        chooser->launchAsync(
            flags,
            [this](const juce::FileChooser& fc) {
                auto result = fc.getResult();
                if (result != juce::File()) {
                    if (result.getFileExtension() != ".xml") {
                        result = result.withFileExtension(".xml");
                    }

                    juce::MemoryBlock dummy;
                    processor.getStateInformation(dummy);

                    const juce::XmlElement xml = processor.settings.createXml();
                    if (xml.writeTo(result)) {
                        currentPresetFile = result;
                        refreshPresets();
                    } else {
                        juce::AlertWindow::showMessageBoxAsync(
                            juce::MessageBoxIconType::WarningIcon,
                            "Error",
                            "Failed to write settings to file."
                        );
                    }
                }
            }
        );
    };

    buttonDelete.setButtonText("Delete");
    buttonDelete.onClick = [this] {
        if (currentPresetFile == juce::File()) {
            return;
        }

        const int selectedId = comboPresets.getSelectedId();
        if (selectedId == 2) {
            // Delete User Default and flip to Factory Default
            if (currentPresetFile.deleteFile()) {
                currentPresetFile = juce::File();
                comboPresets.setSelectedId(1, juce::NotificationType::sendNotification);
                refreshPresets();
            }
        } else if (selectedId > 3) {
            // Delete general preset
            if (currentPresetFile.deleteFile()) {
                currentPresetFile = juce::File();
                comboPresets.setSelectedId(1, juce::NotificationType::sendNotification);
                refreshPresets();
            }
        }
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
    tweakPanel.addAndMakeVisible(comboPresets);
    tweakPanel.addAndMakeVisible(buttonSave);
    tweakPanel.addAndMakeVisible(buttonSaveAs);
    tweakPanel.addAndMakeVisible(buttonDelete);

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

    // Disable Save and Delete buttons for Factory Default
    const int selectedId = comboPresets.getSelectedId();
    buttonSave.setEnabled(selectedId != 1);
    buttonDelete.setEnabled(selectedId != 1);

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
            textWidth = juce::jmax(
                juce::GlyphArrangement::getStringWidth(font, "STFT"),
                juce::GlyphArrangement::getStringWidth(font, "CQT")
            );
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

        positionButtonRight(buttonDelete, panelBounds);
        positionButtonRight(buttonSaveAs, panelBounds);
        positionButtonRight(buttonSave, panelBounds);
        
        // Position the dropdown to the left of the Save buttons
        const int comboWidth = 140;
        comboPresets.setBounds(panelBounds.removeFromRight(comboWidth).reduced(2));
    }
}

void Control::refreshPresets() {
    comboPresets.clear(juce::NotificationType::dontSendNotification);
    comboPresets.addItem("Factory Default", 1);
    
    // User Default is always available now, falling back to Factory if file is missing
    comboPresets.addItem("User Default", 2);
    
    comboPresets.addSeparator();
    comboPresets.addItem("File...", 3);
    comboPresets.addSeparator();

    presets.clear();
    const auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("Pitchenga");
    if (appDataDir.exists()) {
        juce::Array<juce::File> files;
        appDataDir.findChildFiles(files, juce::File::findFiles, false, "*.xml");

        int id = 4;
        for (auto& file : files) {
            // Exclude user-default.xml from the general list as it has its own item
            if (file.getFileName() != "user-default.xml") {
                comboPresets.addItem(file.getFileNameWithoutExtension(), id++);
                presets.push_back(file);
            }
        }
    }


    // Maintain current selection text if applicable
    if (currentPresetFile.existsAsFile()) {
        const auto activeFileName = currentPresetFile.getFileName();
        if (activeFileName == "user-default.xml") {
            comboPresets.setSelectedId(2, juce::NotificationType::dontSendNotification);
            comboPresets.setText("User Default", juce::NotificationType::dontSendNotification);
        } else {
            // Find by filename
            const auto activeNameNoExt = currentPresetFile.getFileNameWithoutExtension();
            bool found = false;
            for (int i = 0; i < comboPresets.getNumItems(); ++i) {
                if (comboPresets.getItemText(i) == activeNameNoExt) {
                    comboPresets.setSelectedItemIndex(i, juce::NotificationType::dontSendNotification);
                    comboPresets.setText(activeNameNoExt, juce::NotificationType::dontSendNotification);
                    found = true;
                    break;
                }
            }
            if (!found) {
                 comboPresets.setText(activeNameNoExt, juce::NotificationType::dontSendNotification);
            }
        }
    }
}

juce::XmlElement Control::Settings::createXml() const {
    // Create an XML element to store settings
    juce::XmlElement xml(getSettingsTagName());

    // Add width and height as attributes
    xml.setAttribute("uiWidth", lastUiWidth);
    xml.setAttribute("uiHeight", lastUiHeight);

    xml.setAttribute("isShowRoll", isShowRoll);
    xml.setAttribute("isShowEye", isShowEye);
    xml.setAttribute("isShowNeedle", isShowNeedle);

    xml.setAttribute("isUseRollStft", isUseRollStft);
    xml.setAttribute("isFreezeRoll", isFreezeRoll);
    xml.setAttribute("isShowForrest", isShowForrest);
    xml.setAttribute("isShowSteam", isShowSteam);

    xml.setAttribute("isEarEnabled", isEarEnabled);
    xml.setAttribute("isShowTweakPanel", isShowTweakPanel);

    if (externalPluginDescriptionXml.isNotEmpty()) {
        xml.createNewChildElement("ExternalPluginDescription")->addTextElement(externalPluginDescriptionXml);
    }

    if (externalPluginStateBase64.isNotEmpty()) {
        xml.createNewChildElement("ExternalPluginState")->setAttribute("base64", externalPluginStateBase64);
    }
    
    xml.setAttribute("isExternalPluginWindowOpen", isExternalPluginWindowOpen);

    xml.setAttribute("splitRatio", splitRatio);

    return xml;
}

bool Control::Settings::loadFromXml(const juce::XmlElement& xml) {
    // Only if the tag matches our expected name
    if (!xml.hasTagName(getSettingsTagName())) return false;

    // Update the processor variables.
    // If the attributes don't exist, it keeps the defaults initialized in .h
    lastUiWidth = xml.getIntAttribute("uiWidth", lastUiWidth);
    lastUiHeight = xml.getIntAttribute("uiHeight", lastUiHeight);

    isShowRoll = xml.getBoolAttribute("isShowRoll", isShowRoll);
    isShowEye = xml.getBoolAttribute("isShowEye", isShowEye);
    isShowNeedle = xml.getBoolAttribute("isShowNeedle", isShowNeedle);

    isUseRollStft = xml.getBoolAttribute("isUseRollStft", isUseRollStft);
    isFreezeRoll = xml.getBoolAttribute("isFreezeRoll", isFreezeRoll);
    isShowForrest = xml.getBoolAttribute("isShowForrest", isShowForrest);
    isShowSteam = xml.getBoolAttribute("isShowSteam", isShowSteam);

    isEarEnabled = xml.getBoolAttribute("isEarEnabled", isEarEnabled);
    isShowTweakPanel = xml.getBoolAttribute("isShowTweakPanel", isShowTweakPanel);

    externalPluginDescriptionXml = {};
    if (auto* descriptionXmlElement = xml.getChildByName("ExternalPluginDescription")) {
        externalPluginDescriptionXml = descriptionXmlElement->getAllSubText();
    }

    externalPluginStateBase64 = {};
    if (auto* stateXmlElement = xml.getChildByName("ExternalPluginState")) {
        externalPluginStateBase64 = stateXmlElement->getStringAttribute("base64");
    }
    
    isExternalPluginWindowOpen = xml.getBoolAttribute("isExternalPluginWindowOpen", isExternalPluginWindowOpen);

    splitRatio = static_cast<float>(xml.getDoubleAttribute("splitRatio", splitRatio));

    return true;
}
