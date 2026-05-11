#include "Control.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include "../PluginProcessor.h"
#include "../DesktopAudioCapture.h"
#include "../Util.h"
#include "BinaryData.h"
#include "version.h"

struct Control::PluginListListener : juce::ChangeListener {
    explicit PluginListListener(Control& ownerControl) : owner(ownerControl) {}

    void changeListenerCallback(juce::ChangeBroadcaster*) override {
        if (owner.isRescanning) {
            // Restart the timer every time the list changes.
            // This acts as a debounce so we only refresh after the scan has settled.
            owner.startTimer(1000);
        }
    }

    Control& owner;
};

// --- Settings Persistence Helpers ---
#define PITCHENGA_MACRO_STRING2(x) #x
#define PITCHENGA_MACRO_STRING(x) PITCHENGA_MACRO_STRING2(x)

static juce::String getSettingsTagName() {
    return "PITCHENGA";
}

Control::VolumeKnob::VolumeKnob() {
    setSliderStyle(RotaryHorizontalVerticalDrag);
    setTextBoxStyle(NoTextBox, false, 0, 0);
    setRange(0.0, 1.0);
    SettableTooltipClient::setTooltip("Monitor Volume (Click to toggle)");
}

void Control::VolumeKnob::paint(juce::Graphics& graphics) {
    auto bounds = getLocalBounds().toFloat();

    const float radius = std::min(bounds.getWidth(), bounds.getHeight()) * 0.5f - 2.0f;
    const float centerX = bounds.getCentreX();
    const float centerY = bounds.getCentreY();

    const auto currentVolume = static_cast<float>(getValue());

    // Background
    graphics.setColour(juce::Colours::black);
    graphics.fillEllipse(centerX - radius, centerY - radius, radius * 2.0f, radius * 2.0f);

    // Outline
    graphics.setColour(juce::Colours::grey);
    graphics.drawEllipse(centerX - radius, centerY - radius, radius * 2.0f, radius * 2.0f, 1.0f);

    constexpr float startAngle = -juce::MathConstants<float>::pi * 0.75f;
    const float endAngle = startAngle + (currentVolume * juce::MathConstants<float>::pi * 1.5f);

    // Fill Arc
    if (currentVolume > 0.0f) {
        juce::Path volumeArc;
        volumeArc.addCentredArc(centerX, centerY, radius * 0.7f, radius * 0.7f, 0.0f, startAngle, endAngle, true);
        graphics.setColour(juce::Colours::white);
        graphics.strokePath(volumeArc, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved));
    }

    // Pointer
    const float pointerX = centerX + std::sin(endAngle) * radius * 0.7f;
    const float pointerY = centerY - std::cos(endAngle) * radius * 0.7f;

    graphics.setColour(currentVolume > 0.0f ? juce::Colours::white : juce::Colours::grey);
    graphics.drawLine(centerX, centerY, pointerX, pointerY, 2.0f);
}

void Control::VolumeKnob::mouseDown(const juce::MouseEvent& event) {
    Slider::mouseDown(event);
    wasDragged = false;
}

void Control::VolumeKnob::mouseDrag(const juce::MouseEvent& event) {
    Slider::mouseDrag(event);
    if (event.mouseWasDraggedSinceMouseDown()) {
        wasDragged = true;
    }
}

void Control::VolumeKnob::mouseUp(const juce::MouseEvent& event) {
    Slider::mouseUp(event);
    if (!wasDragged) {
        setValue(getValue() > 0.0 ? 0.0 : 1.0, juce::sendNotificationSync);
    }
}

Control::Control(PitchengaAudioProcessor& proc)
    : processor(proc),
    listListener(std::make_unique<PluginListListener>(*this)) {
    processor.getKnownPluginList().addChangeListener(listListener.get());

    setupToggleButton(toggleRoll, processor.settings.isShowRoll);
    toggleRoll.onClick = [this] {
        processor.settings.isShowRoll = toggleRoll.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
        updateButtonStates();
    };

    setupToggleButton(toggleEye, processor.settings.isShowEye);
    toggleEye.onClick = [this] {
        processor.settings.isShowEye = toggleEye.getToggleState();
        updateButtonStates();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleNeedle, processor.settings.isShowNeedle);
    toggleNeedle.onClick = [this] {
        processor.settings.isShowNeedle = toggleNeedle.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleLayoutPivot, processor.settings.isLayoutHorizontal);
    toggleLayoutPivot.onClick = [this] {
        processor.settings.isLayoutHorizontal = toggleLayoutPivot.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleSmoke, processor.settings.isShowSmoke);
    toggleSmoke.onClick = [this] {
        processor.settings.isShowSmoke = toggleSmoke.getToggleState();
    };

    setupToggleButton(toggleIsFreezeRoll, processor.settings.isFreezeRoll);
    toggleIsFreezeRoll.onClick = [this] {
        processor.settings.isFreezeRoll = toggleIsFreezeRoll.getToggleState();
    };

    setupToggleButton(toggleStrobe, processor.settings.isShowStrobe);
    toggleStrobe.onClick = [this] {
        processor.settings.isShowStrobe = toggleStrobe.getToggleState();
    };

    setupToggleButton(toggleRaw, processor.settings.isRawMode);
    toggleRaw.onClick = [this] {
        processor.settings.isRawMode = toggleRaw.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
        updateButtonStates();
    };

    setupToggleButton(toggleLetter, !processor.settings.isLetterNotation);
    toggleLetter.setButtonText(processor.settings.isLetterNotation ? letter : solfege);
    toggleLetter.onClick = [this] {
        processor.settings.isLetterNotation = !toggleLetter.getToggleState();
        toggleLetter.setButtonText(processor.settings.isLetterNotation ? letter : solfege);
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleRollType, processor.settings.isUseRollStft);
    toggleRollType.setButtonText(processor.settings.isUseRollStft ? stft : cqt);
    toggleRollType.onClick = [this] {
        processor.settings.isUseRollStft = toggleRollType.getToggleState();
        toggleRollType.setButtonText(processor.settings.isUseRollStft ? stft : cqt);
        updateButtonStates();
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleFlipRoll, processor.settings.isFlipRollHorizontal);
    toggleFlipRoll.setButtonText(processor.settings.isFlipRollHorizontal ? flip : flop);
    toggleFlipRoll.onClick = [this] {
        processor.settings.isFlipRollHorizontal = toggleFlipRoll.getToggleState();
        toggleFlipRoll.setButtonText(processor.settings.isFlipRollHorizontal ? flip : flop);
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleForrest, processor.settings.isShowForrest);
    toggleForrest.onClick = [this] {
        processor.settings.isShowForrest = toggleForrest.getToggleState();
    };

    micLabel.setText(mic, juce::NotificationType::dontSendNotification);
    micLabel.setFont(juce::FontOptions(13.0f).withStyle("Bold"));
    micLabel.setJustificationType(juce::Justification::centredLeft);
    micLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    micLabel.setBorderSize(juce::BorderSize(0));

    volumeLabelLeft.setFont(juce::FontOptions(13.0f).withStyle("Bold"));
    volumeLabelLeft.setJustificationType(juce::Justification::centredLeft);
    volumeLabelLeft.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeLabelLeft.setBorderSize(juce::BorderSize(0));

    volumeLabelRight.setFont(juce::FontOptions(13.0f).withStyle("Bold"));
    volumeLabelRight.setJustificationType(juce::Justification::centredLeft);
    volumeLabelRight.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeLabelRight.setBorderSize(juce::BorderSize(0));

    auto updateVolumeLabel = [](const juce::Slider& knob, juce::Label& label) {
        const auto currentVolume = static_cast<float>(knob.getValue());
        juce::String volumeText;
        if (currentVolume <= 0.0001f) {
            volumeText = "-inf";
            label.setColour(juce::Label::textColourId, juce::Colours::grey);
        } else {
            float decibels = 20.0f * std::log10(currentVolume);
            if (decibels > -0.1f) decibels = 0.0f;
            volumeText = juce::String(decibels, 1);
            label.setColour(juce::Label::textColourId, juce::Colours::white);
        }
        label.setText(volumeText, juce::NotificationType::dontSendNotification);
    };

    knobEarLeft.setValue(processor.settings.earVolumeLeft, juce::NotificationType::dontSendNotification);
    updateVolumeLabel(knobEarLeft, volumeLabelLeft);
    knobEarLeft.onValueChange = [this, updateVolumeLabel] {
        processor.settings.earVolumeLeft = static_cast<float>(knobEarLeft.getValue());
        updateVolumeLabel(knobEarLeft, volumeLabelLeft);
    };

    knobEarRight.setValue(processor.settings.earVolumeRight, juce::NotificationType::dontSendNotification);
    updateVolumeLabel(knobEarRight, volumeLabelRight);
    knobEarRight.onValueChange = [this, updateVolumeLabel] {
        processor.settings.earVolumeRight = static_cast<float>(knobEarRight.getValue());
        updateVolumeLabel(knobEarRight, volumeLabelRight);
    };

    setupToggleButton(toggleCapture, processor.settings.isCaptureEnabled);
    toggleCapture.onClick = [this] {
        processor.settings.isCaptureEnabled = toggleCapture.getToggleState();
        if (processor.settings.isCaptureEnabled) {
            processor.startDesktopCapture();
        } else {
            processor.stopDesktopCapture();
        }
    };

    setupButton(buttonPlugs);
    buttonPlugs.setButtonText(plugs);
    buttonPlugs.onClick = [this] {
        showPlugsMenu();
    };

    setupButton(buttonPlug);
    buttonPlug.setButtonText(plug);
    buttonPlug.onClick = [this] {
        processor.showExternalPluginEditor();
    };

    comboPresets.setTextWhenNothingSelected(presetsComboTextWhenNothingSelected);
    comboPresets.onChange = [this] {
        const int id = comboPresets.getSelectedId();
        if (id == nonePresetId) return;

        std::unique_ptr<juce::XmlElement> xml;
        juce::File newPresetFile;
        juce::String newPresetName;

        if (id == factoryDefaultPresetId) {
            newPresetFile = juce::File();
            newPresetName = factoryDefaultPresetName;
            xml = juce::XmlDocument::parse(
                juce::String::createStringFromData(
                    BinaryData::factorysettings_xml,
                    BinaryData::factorysettings_xmlSize
                )
            );
            if (xml != nullptr) xml->setTagName(getSettingsTagName());
        } else if (id == userDefaultPresetId) {
            const auto appDataDir = Util::getApplicationDirectory();
            newPresetFile = appDataDir.getChildFile(presetsDirectoryName).getChildFile(userDefaultPresetFileName);
            newPresetName = userDefaultPresetName;

            if (newPresetFile.existsAsFile()) {
                xml = juce::XmlDocument::parse(newPresetFile);
            } else {
                // Fallback to factory if missing, but keep User Default context for Saving
                xml = juce::XmlDocument::parse(
                    juce::String::createStringFromData(
                        BinaryData::factorysettings_xml,
                        BinaryData::factorysettings_xmlSize
                    )
                );
                if (xml != nullptr) xml->setTagName(getSettingsTagName());
            }
        } else if (id >= customPresetsStartId) {
            const auto index = static_cast<size_t>(id - customPresetsStartId);
            if (index < presets.size()) {
                newPresetFile = presets[index];
                newPresetName = newPresetFile.getFileNameWithoutExtension();
                xml = juce::XmlDocument::parse(newPresetFile);
            }
        }

        if (xml != nullptr) {
            juce::MemoryBlock destData;
            PitchengaAudioProcessor::copyXmlToBinary(*xml, destData);

            // Centralized state restoration: handles settings, window size, and plugins
            processor.setStateInformation(destData.getData(), static_cast<int>(destData.getSize()));

            currentPresetFile = newPresetFile;
            processor.settings.currentPresetName = newPresetName;
            comboPresets.setText(newPresetName, juce::NotificationType::dontSendNotification);
        }

        updateButtonStates();
    };

    refreshPresets();

    // Ensure Default.xml exists, create from factory if missing
    const auto appDataDir = Util::getApplicationDirectory();
    const auto presetsDir = appDataDir.getChildFile(presetsDirectoryName);
    const auto defaultFile = presetsDir.getChildFile(userDefaultPresetFileName);
    if (!defaultFile.existsAsFile()) {
        const auto factoryXml = juce::XmlDocument::parse(
            juce::String::createStringFromData(
                BinaryData::factorysettings_xml,
                BinaryData::factorysettings_xmlSize
            )
        );
        if (factoryXml != nullptr) {
            factoryXml->setTagName(getSettingsTagName());
            if (presetsDir.createDirectory()) {
                factoryXml->writeTo(defaultFile);
            }
        }
    }

    // Restore selection by name
    if (processor.settings.currentPresetName.isNotEmpty()) {
        const auto presetName = processor.settings.currentPresetName;
        if (presetName == userDefaultPresetName) {
            comboPresets.setSelectedId(userDefaultPresetId, juce::NotificationType::dontSendNotification);
            currentPresetFile = presetsDir.getChildFile(userDefaultPresetFileName);
        } else if (presetName == factoryDefaultPresetName) {
            comboPresets.setSelectedId(factoryDefaultPresetId, juce::NotificationType::dontSendNotification);
            currentPresetFile = juce::File();
        } else {
            for (size_t i = 0; i < presets.size(); ++i) {
                if (presets[i].getFileNameWithoutExtension() == presetName) {
                    // Item IDs for general presets start at 4
                    comboPresets.setSelectedId(
                        static_cast<int>(i) + customPresetsStartId,
                        juce::NotificationType::dontSendNotification
                    );
                    currentPresetFile = presets[i];
                    break;
                }
            }
        }
        comboPresets.setText(presetName, juce::NotificationType::dontSendNotification);
    }

    setupToggleButton(toggleTweak, processor.settings.isShowTweakPanel);
    toggleTweak.onClick = [this] {
        processor.settings.isShowTweakPanel = toggleTweak.getToggleState();
        if (onVisibilityChanged) onVisibilityChanged();
        updateButtonStates();
    };

    setupButton(buttonSave);
    buttonSave.setButtonText(save);
    buttonSave.onClick = [this] {
        auto presetName = comboPresets.getText();
        if (presetName == factoryDefaultPresetName || currentPresetFile == juce::File()) {
            presetName = userDefaultPresetName;
        }
        juce::Component::SafePointer<Control> safeThis(this);
        juce::AlertWindow::showOkCancelBox(
            juce::MessageBoxIconType::QuestionIcon,
            saveConfirmTitle,
            saveConfirmMessage.replace("{NAME}", presetName),
            save,
            "Cancel",
            nullptr,
            juce::ModalCallbackFunction::create(
                [safeThis, presetName](int result) {
                    if (result != 0 && safeThis != nullptr) {
                        if (presetName == safeThis->userDefaultPresetName) {
                            safeThis->currentPresetFile = Util::getApplicationDirectory()
                                .getChildFile(safeThis->presetsDirectoryName)
                                .getChildFile(safeThis->userDefaultPresetFileName);
                        }
                        safeThis->saveCurrentPreset();
                    }
                }
            )
        );
    };

    setupButton(buttonSaveAs);
    buttonSaveAs.setButtonText(saveAs);
    buttonSaveAs.onClick = [this] {
        const juce::String currentName = comboPresets.getText();
        juce::String suggestedName = currentName;

        if (currentName == ""
            || currentName == presetsComboTextWhenNothingSelected
            || currentName == factoryDefaultPresetName
            || currentName == userDefaultPresetName
        ) {
            suggestedName = userDefaultPresetFileName;
        }

        chooser = std::make_unique<juce::FileChooser>(
            "Select where to save the settings...",
            Util::getApplicationDirectory().getChildFile(presetsDirectoryName).getChildFile(suggestedName),
            "*.xml"
        );

        auto flags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles |
            juce::FileBrowserComponent::warnAboutOverwriting;

        juce::Component::SafePointer<Control> safeThis(this);
        chooser->launchAsync(
            flags,
            [safeThis](const juce::FileChooser& fc) {
                auto result = fc.getResult();
                if (result != juce::File() && safeThis != nullptr) {
                    if (result.getFileExtension() != ".xml") {
                        result = result.withFileExtension(".xml");
                    }

                    juce::MemoryBlock dummy;
                    safeThis->processor.getStateInformation(dummy);

                    const juce::XmlElement xml = safeThis->processor.settings.createXml();
                    if (result.getParentDirectory().createDirectory() && xml.writeTo(result)) {
                        safeThis->currentPresetFile = result;
                        safeThis->processor.settings.currentPresetName = result.getFileNameWithoutExtension();
                        safeThis->refreshPresets();
                        safeThis->updateButtonStates();
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

    setupButton(buttonDelete);
    buttonDelete.setButtonText(deletePreset);
    buttonDelete.onClick = [this] {
        if (currentPresetFile == juce::File()) {
            return;
        }

        juce::Component::SafePointer<Control> safeThis(this);
        juce::AlertWindow::showOkCancelBox(
            juce::MessageBoxIconType::QuestionIcon,
            deleteConfirmTitle,
            deleteConfirmMessage.replace("{NAME}", comboPresets.getText()),
            deletePreset,
            "Cancel",
            nullptr,
            juce::ModalCallbackFunction::create(
                [safeThis](int result) {
                    if (result != 0 && safeThis != nullptr) {
                        safeThis->deleteCurrentPreset();
                    }
                }
            )
        );
    };

    setupButton(buttonRename);
    buttonRename.setButtonText(rename);
    buttonRename.onClick = [this] {
        if (currentPresetFile == juce::File()) {
            return;
        }
        renameCurrentPreset();
    };

    addAndMakeVisible(toggleNeedle);
    addAndMakeVisible(toggleEye);
    addAndMakeVisible(toggleRoll);
    addAndMakeVisible(toggleLayoutPivot);
    addAndMakeVisible(toggleIsFreezeRoll);
    addAndMakeVisible(toggleRaw);

    addAndMakeVisible(toggleTweak);
    addAndMakeVisible(comboPresets);
    addAndMakeVisible(buttonSave);
    addAndMakeVisible(buttonSaveAs);

    addAndMakeVisible(tweakPanel);
    tweakPanel.addAndMakeVisible(buttonPlugs);
    tweakPanel.addAndMakeVisible(buttonPlug);
    tweakPanel.addAndMakeVisible(toggleCapture);

    tweakPanel.addAndMakeVisible(micLabel);
    tweakPanel.addAndMakeVisible(knobEarLeft);
    tweakPanel.addAndMakeVisible(knobEarRight);
    tweakPanel.addAndMakeVisible(volumeLabelLeft);
    tweakPanel.addAndMakeVisible(volumeLabelRight);

    tweakPanel.addAndMakeVisible(toggleStrobe);
    tweakPanel.addAndMakeVisible(toggleRollType);
    tweakPanel.addAndMakeVisible(toggleFlipRoll);
    tweakPanel.addAndMakeVisible(toggleSmoke);
    tweakPanel.addAndMakeVisible(toggleForrest);
    tweakPanel.addAndMakeVisible(toggleLetter);
    tweakPanel.addAndMakeVisible(buttonDelete);
    tweakPanel.addAndMakeVisible(buttonRename);

    buildTimestampLabel.setText(VERSION, juce::NotificationType::dontSendNotification);
    buildTimestampLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    buildTimestampLabel.setFont(juce::FontOptions(13.0f));
    buildTimestampLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(buildTimestampLabel);

    updateButtonStates();
}

Control::~Control() {
    processor.getKnownPluginList().removeChangeListener(listListener.get());
}

void Control::showPlugsMenu() {
    juce::PopupMenu menu;
    menu.addItem(1, "Open Plugins Browser...");
    menu.addItem(2, "Rescan Plugins");
    menu.addSeparator();
    menu.addItem(3, "Unload Plugin", processor.isExternalPluginLoaded());
    menu.addSeparator();

    auto& list = processor.getKnownPluginList();
    auto types = list.getTypes();

    int id = 4;
    for (auto&& type : types) {
        if (type.isInstrument) {
            menu.addItem(id, type.name);
        }
        id++;
    }

    menu.showMenuAsync(
        juce::PopupMenu::Options().withTargetComponent(&buttonPlugs),
        [this](int result) {
            if (result == 1) {
                processor.openPluginBrowser();
            } else if (result == 2) {
                isRescanning = true;
                processor.rescanPlugins();
                showPlugsMenu();
            } else if (result == 3) {
                processor.unloadExternalPlugin();
            } else if (result > 3) {
                auto& listRef = processor.getKnownPluginList();
                auto typesRef = listRef.getTypes();
                const int index = result - 4;
                if (index >= 0 && index < typesRef.size()) {
                    processor.loadExternalPlugin(typesRef[static_cast<size_t>(index)], true);
                }
            }
        }
    );
}

void Control::timerCallback() {
    stopTimer();
    isRescanning = false;
    showPlugsMenu();
}

void Control::saveCurrentPreset() {
    // Sync current plugin state into settings first
    juce::MemoryBlock dummy;
    processor.getStateInformation(dummy);

    const juce::XmlElement xml = processor.settings.createXml();
    if (currentPresetFile.getParentDirectory().createDirectory() && xml.writeTo(currentPresetFile)) {
        refreshPresets();
        // Explicitly force text update for transitions like Factory -> User Default
        if (currentPresetFile.getFileName() == userDefaultPresetFileName) {
            comboPresets.setText(userDefaultPresetName, juce::NotificationType::dontSendNotification);
        } else {
            comboPresets.setText(
                currentPresetFile.getFileNameWithoutExtension(),
                juce::NotificationType::dontSendNotification
            );
        }
    } else {
        juce::AlertWindow::showMessageBoxAsync(
            juce::MessageBoxIconType::WarningIcon,
            "Error",
            "Failed to overwrite setup file."
        );
    }
}

void Control::deleteCurrentPreset() {
    const int selectedId = comboPresets.getSelectedId();
    if (selectedId >= customPresetsStartId) {
        if (currentPresetFile.deleteFile()) {
            currentPresetFile = juce::File();
            processor.settings.currentPresetName = "";
            comboPresets.setSelectedId(factoryDefaultPresetId, juce::NotificationType::sendNotification);
            refreshPresets();
        }
    }
}

void Control::renameCurrentPreset() {
    auto alert = std::make_shared<juce::AlertWindow>(
        "Rename Preset",
        "Enter a new name for the preset:",
        juce::MessageBoxIconType::QuestionIcon
    );

    alert->addTextEditor("name", currentPresetFile.getFileNameWithoutExtension(), "New name");
    alert->addButton("Rename", 1, juce::KeyPress(juce::KeyPress::returnKey));
    alert->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    juce::Component::SafePointer<Control> safeThis(this);
    auto* rawAlert = alert.get();
    rawAlert->enterModalState(
        true,
        juce::ModalCallbackFunction::create(
            [safeThis, alert](int result) {
                if (result == 1 && safeThis != nullptr) {
                    juce::String newName = alert->getTextEditorContents("name").trim();
                    if (newName.isNotEmpty() && newName != safeThis->currentPresetFile.getFileNameWithoutExtension()) {
                        juce::File newFile = safeThis->currentPresetFile.getSiblingFile(newName)
                            .withFileExtension(".xml");

                        auto doRename = [safeThis, newFile, newName]() {
                            if (newFile.existsAsFile() && newFile != safeThis->currentPresetFile) {
                                if (!newFile.deleteFile()) {
                                    juce::AlertWindow::showMessageBoxAsync(
                                        juce::MessageBoxIconType::WarningIcon,
                                        "Error",
                                        "Failed to overwrite existing preset."
                                    );
                                    return;
                                }
                            }
                            if (safeThis->currentPresetFile.moveFileTo(newFile)) {
                                safeThis->currentPresetFile = newFile;
                                safeThis->processor.settings.currentPresetName = newName;
                                safeThis->refreshPresets();
                                safeThis->updateButtonStates();
                            } else {
                                juce::AlertWindow::showMessageBoxAsync(
                                    juce::MessageBoxIconType::WarningIcon,
                                    "Error",
                                    "Failed to rename preset file."
                                );
                            }
                        };

                        if (newFile.existsAsFile() && newFile != safeThis->currentPresetFile) {
                            juce::AlertWindow::showOkCancelBox(
                                juce::MessageBoxIconType::QuestionIcon,
                                "Overwrite Preset",
                                "A preset with the name '" + newName + "' already exists. Overwrite?",
                                "Overwrite",
                                "Cancel",
                                nullptr,
                                juce::ModalCallbackFunction::create(
                                    [doRename](int overwriteResult) {
                                        if (overwriteResult != 0) {
                                            doRename();
                                        }
                                    }
                                )
                            );
                        } else {
                            doRename();
                        }
                    }
                }
            }
        )
    );
}

Control::NoEllipsisLookAndFeel::NoEllipsisLookAndFeel() {
    setColour(juce::TextButton::buttonColourId, juce::Colours::black.withAlpha(0.4f));
    setColour(juce::TextButton::buttonOnColourId, juce::Colours::white.withAlpha(0.2f));
    setColour(juce::TextButton::textColourOffId, juce::Colours::white.withAlpha(0.5f));
    setColour(juce::TextButton::textColourOnId, juce::Colours::white);
}

void Control::NoEllipsisLookAndFeel::drawButtonText(juce::Graphics& graphics, juce::TextButton& button, bool, bool) {
    graphics.setFont(getTextButtonFont(button, button.getHeight()));

    const bool isToggle = button.getClickingTogglesState();
    const bool isOn = button.getToggleState();

    // Use On colour for non-toggle buttons or when a toggle button is active
    auto colour = button.findColour(
        (!isToggle || isOn)
            ? juce::TextButton::textColourOnId
            : juce::TextButton::textColourOffId
    );

    graphics.setColour(colour.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

    const int yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
    const int textWidth = button.getWidth();

    if (textWidth > 0) {
        // Use drawText with useEllipsis = false to prevent "..." truncation
        graphics.drawText(
            button.getButtonText(),
            0,
            yIndent,
            textWidth,
            button.getHeight() - yIndent * 2,
            juce::Justification::centred,
            false
        );
    }
}

juce::Font Control::NoEllipsisLookAndFeel::getTextButtonFont(juce::TextButton&, int) {
    return juce::FontOptions(15.0f).withStyle("Bold");
}

void Control::setupButton(juce::TextButton& button) {
    button.setLookAndFeel(&noEllipsisLookAndFeel);
    button.setConnectedEdges(
        juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight |
        juce::Button::ConnectedOnTop | juce::Button::ConnectedOnBottom
    );
}

void Control::setupToggleButton(juce::TextButton& button, bool initialState) {
    setupButton(button);
    button.setClickingTogglesState(true);
    button.setToggleState(initialState, juce::NotificationType::dontSendNotification);
}

void Control::updateVisibilityFromState() {
    toggleRoll.setToggleState(processor.settings.isShowRoll, juce::NotificationType::dontSendNotification);
    toggleEye.setToggleState(processor.settings.isShowEye, juce::NotificationType::dontSendNotification);
    toggleNeedle.setToggleState(processor.settings.isShowNeedle, juce::NotificationType::dontSendNotification);
    toggleLayoutPivot.setToggleState(
        processor.settings.isLayoutHorizontal,
        juce::NotificationType::dontSendNotification
    );
    toggleIsFreezeRoll.setToggleState(processor.settings.isFreezeRoll, juce::NotificationType::dontSendNotification);

    toggleRollType.setToggleState(processor.settings.isUseRollStft, juce::NotificationType::dontSendNotification);
    toggleRollType.setButtonText(processor.settings.isUseRollStft ? stft : cqt);
    toggleFlipRoll.setToggleState(
        processor.settings.isFlipRollHorizontal,
        juce::NotificationType::dontSendNotification
    );
    toggleFlipRoll.setButtonText(processor.settings.isFlipRollHorizontal ? flip : flop);
    toggleStrobe.setToggleState(processor.settings.isShowStrobe, juce::NotificationType::dontSendNotification);
    toggleRaw.setToggleState(processor.settings.isRawMode, juce::NotificationType::dontSendNotification);
    toggleLetter.setToggleState(!processor.settings.isLetterNotation, juce::NotificationType::dontSendNotification);
    toggleLetter.setButtonText(processor.settings.isLetterNotation ? letter : solfege);
    toggleSmoke.setToggleState(processor.settings.isShowSmoke, juce::NotificationType::dontSendNotification);
    toggleForrest.setToggleState(processor.settings.isShowForrest, juce::NotificationType::dontSendNotification);

    knobEarLeft.setValue(processor.settings.earVolumeLeft, juce::NotificationType::dontSendNotification);
    knobEarRight.setValue(processor.settings.earVolumeRight, juce::NotificationType::dontSendNotification);
    toggleCapture.setToggleState(processor.settings.isCaptureEnabled, juce::NotificationType::dontSendNotification);
    toggleTweak.setToggleState(processor.settings.isShowTweakPanel, juce::NotificationType::dontSendNotification);

    updateButtonStates();
}

void Control::updateButtonStates() {
    const bool isStandalone = processor.wrapperType == juce::AudioProcessor::wrapperType_Standalone;
    buttonPlugs.setVisible(isStandalone);
    buttonPlug.setVisible(isStandalone);
    micLabel.setVisible(isStandalone);
    knobEarLeft.setVisible(isStandalone);
    knobEarRight.setVisible(isStandalone);
    toggleCapture.setVisible(isStandalone && DesktopAudioCapture::isSupported());

    const bool rollActive = processor.settings.isShowRoll;
    toggleIsFreezeRoll.setVisible(rollActive);
    toggleRollType.setVisible(rollActive);
    toggleRaw.setVisible(rollActive);
    toggleRaw.setEnabled(processor.settings.isUseRollStft);
    toggleStrobe.setVisible(processor.settings.isShowNeedle);
    toggleLayoutPivot.setEnabled(rollActive && processor.settings.isShowEye);
    toggleFlipRoll.setVisible(rollActive);
    toggleSmoke.setVisible(rollActive);
    toggleForrest.setVisible(rollActive);

    buttonPlug.setEnabled(processor.isExternalPluginLoaded());

    tweakPanel.setVisible(processor.settings.isShowTweakPanel);

    // Disable Save and Delete buttons for Factory (ID 1) and nothing selected (ID 0)
    const int selectedId = comboPresets.getSelectedId();
    buttonSave.setEnabled(selectedId > nonePresetId);
    buttonDelete.setEnabled(selectedId >= customPresetsStartId);
    buttonRename.setEnabled(selectedId >= customPresetsStartId);

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
    const int rowHeight = static_cast<int>(font.getHeight() + 8.0f);

    // Create the top row
    auto topRow = bounds.removeFromTop(rowHeight);

    // Pack the buttons to the left with minimal offsets dynamically sizing to their text
    auto positionButton = [&](juce::TextButton& button, juce::Rectangle<int>& container) {
        if (!button.isVisible()) return;
        const float textWidth = juce::GlyphArrangement::getStringWidth(font, button.getButtonText());
        const int buttonWidth = static_cast<int>(std::ceil(textWidth)) + 8;
        button.setBounds(container.removeFromLeft(buttonWidth));
    };

    auto positionButtonRight = [&](juce::TextButton& button, juce::Rectangle<int>& container) {
        if (!button.isVisible()) return;
        float textWidth = juce::GlyphArrangement::getStringWidth(font, button.getButtonText());
        if (&button == &toggleRollType) {
            textWidth = juce::GlyphArrangement::getStringWidth(font, stft);
        } else if (&button == &toggleFlipRoll) {
            textWidth = juce::GlyphArrangement::getStringWidth(font, flop);
        } else if (&button == &toggleLetter) {
            textWidth = juce::GlyphArrangement::getStringWidth(font, solfege);
        }
        const int buttonWidth = static_cast<int>(std::ceil(textWidth)) + 8;
        button.setBounds(container.removeFromRight(buttonWidth));
    };

    positionButton(toggleNeedle, topRow);
    positionButton(toggleEye, topRow);
    positionButton(toggleRoll, topRow);
    positionButton(toggleLayoutPivot, topRow);

    topRow.removeFromLeft(4);
    const float versionWidth = juce::GlyphArrangement::getStringWidth(
        buildTimestampLabel.getFont(),
        buildTimestampLabel.getText()
    );
    buildTimestampLabel.setBounds(topRow.removeFromLeft(static_cast<int>(std::ceil(versionWidth)) + 4));

    // Position presets and tweak from the right
    positionButtonRight(buttonSaveAs, topRow);
    positionButtonRight(buttonSave, topRow);
    constexpr int comboWidth = 140;
    comboPresets.setBounds(topRow.removeFromRight(comboWidth));
    topRow.removeFromRight(6);
    positionButtonRight(toggleTweak, topRow);
    positionButtonRight(toggleRaw, topRow);

    positionButtonRight(toggleIsFreezeRoll, topRow);

    if (processor.settings.isShowTweakPanel) {
        tweakPanel.setBounds(bounds); // Use the entire remaining bounds (the bottom row)

        auto panelBounds = tweakPanel.getLocalBounds();
        positionButton(buttonPlugs, panelBounds);
        positionButton(buttonPlug, panelBounds);
        positionButton(toggleCapture, panelBounds);

        if (processor.wrapperType == juce::AudioProcessor::wrapperType_Standalone) {
            panelBounds.removeFromLeft(6);

            // Layout the "Mic" label
            const float micTextWidth = juce::GlyphArrangement::getStringWidth(micLabel.getFont(), mic);
            const int micWidth = static_cast<int>(std::ceil(micTextWidth)) + 4;
            micLabel.setBounds(panelBounds.removeFromLeft(micWidth));

            // Layout the circular knobs (square, matching rowHeight)
            knobEarLeft.setBounds(panelBounds.removeFromLeft(rowHeight));

            // Layout the label independently with a fixed width based on the maximum string length
            const float maxTextWidth = juce::GlyphArrangement::getStringWidth(volumeLabelLeft.getFont(), "-00.0");
            const int fixedLabelWidth = static_cast<int>(std::ceil(maxTextWidth));
            volumeLabelLeft.setBounds(panelBounds.removeFromLeft(fixedLabelWidth));

            knobEarRight.setBounds(panelBounds.removeFromLeft(rowHeight));
            volumeLabelRight.setBounds(panelBounds.removeFromLeft(fixedLabelWidth));
        }

        positionButtonRight(buttonDelete, panelBounds);
        positionButtonRight(buttonRename, panelBounds);
        panelBounds.removeFromRight(6);

        positionButtonRight(toggleForrest, panelBounds);
        positionButtonRight(toggleSmoke, panelBounds);
        positionButtonRight(toggleFlipRoll, panelBounds);
        positionButtonRight(toggleRollType, panelBounds);

        positionButtonRight(toggleStrobe, panelBounds);
        positionButtonRight(toggleLetter, panelBounds);
    }
}

void Control::refreshPresets() {
    comboPresets.clear(juce::NotificationType::dontSendNotification);
    comboPresets.addItem(factoryDefaultPresetName, factoryDefaultPresetId);

    // User Default is always available now, falling back to Factory if file is missing
    comboPresets.addItem(userDefaultPresetName, userDefaultPresetId);

    comboPresets.addSeparator();

    presets.clear();
    const auto appDataDir = Util::getApplicationDirectory();
    const auto presetsDir = appDataDir.getChildFile(presetsDirectoryName);
    if (presetsDir.exists()) {
        juce::Array<juce::File> files;
        presetsDir.findChildFiles(files, juce::File::findFiles, false, "*.xml");

        int id = customPresetsStartId;
        for (auto& file : files) {
            // Exclude user-default.xml from the general list as it has its own item
            if (file.getFileName() != userDefaultPresetFileName) {
                comboPresets.addItem(file.getFileNameWithoutExtension(), id++);
                presets.push_back(file);
            }
        }
    }


    // Maintain current selection text if applicable
    if (currentPresetFile.existsAsFile()) {
        const auto activeFileName = currentPresetFile.getFileName();
        if (activeFileName == userDefaultPresetFileName) {
            comboPresets.setSelectedId(userDefaultPresetId, juce::NotificationType::dontSendNotification);
            comboPresets.setText(userDefaultPresetName, juce::NotificationType::dontSendNotification);
        } else {
            // Find by filename
            const auto activeNameNoExt = currentPresetFile.getFileNameWithoutExtension();
            bool found = false;
            for (size_t i = 0; i < presets.size(); ++i) {
                if (presets[i].getFileNameWithoutExtension() == activeNameNoExt) {
                    comboPresets.setSelectedId(
                        static_cast<int>(i) + customPresetsStartId,
                        juce::NotificationType::dontSendNotification
                    );
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
    xml.setAttribute("isShowStrobe", isShowStrobe);
    xml.setAttribute("isShowForrest", isShowForrest);
    xml.setAttribute("isShowSmoke", isShowSmoke);
    xml.setAttribute("isFlipRollHorizontal", isFlipRollHorizontal);
    xml.setAttribute("isLayoutHorizontal", isLayoutHorizontal);

    xml.setAttribute("earVolumeLeft", earVolumeLeft);
    xml.setAttribute("earVolumeRight", earVolumeRight);
    xml.setAttribute("isCaptureEnabled", isCaptureEnabled);
    xml.setAttribute("isShowTweakPanel", isShowTweakPanel);
    xml.setAttribute("isRawMode", isRawMode);
    xml.setAttribute("isLetterNotation", isLetterNotation);

    if (externalPluginDescriptionXml.isNotEmpty()) {
        xml.createNewChildElement("ExternalPluginDescription")->addTextElement(externalPluginDescriptionXml);
    }

    if (externalPluginStateBase64.isNotEmpty()) {
        xml.createNewChildElement("ExternalPluginState")->setAttribute("base64", externalPluginStateBase64);
    }

    xml.setAttribute("isExternalPluginWindowOpen", isExternalPluginWindowOpen);

    xml.setAttribute("splitRatio", splitRatio);
    xml.setAttribute("currentPresetName", currentPresetName);

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
    isShowStrobe = xml.getBoolAttribute("isShowStrobe", isShowStrobe);
    isShowForrest = xml.getBoolAttribute("isShowForrest", isShowForrest);
    isShowSmoke = xml.getBoolAttribute("isShowSmoke", isShowSmoke);
    isFlipRollHorizontal = xml.getBoolAttribute("isFlipRollHorizontal", isFlipRollHorizontal);
    isLayoutHorizontal = xml.getBoolAttribute("isLayoutHorizontal", isLayoutHorizontal);

    earVolumeLeft = static_cast<float>(xml.getDoubleAttribute("earVolumeLeft", earVolumeLeft));
    earVolumeRight = static_cast<float>(xml.getDoubleAttribute("earVolumeRight", earVolumeRight));
    isCaptureEnabled = xml.getBoolAttribute("isCaptureEnabled", isCaptureEnabled);
    isShowTweakPanel = xml.getBoolAttribute("isShowTweakPanel", isShowTweakPanel);
    isRawMode = xml.getBoolAttribute("isRawMode", isRawMode);
    isLetterNotation = xml.getBoolAttribute("isLetterNotation", isLetterNotation);

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
    currentPresetName = xml.getStringAttribute("currentPresetName", currentPresetName);

    return true;
}
