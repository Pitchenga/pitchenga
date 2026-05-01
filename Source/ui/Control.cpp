#include "Control.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include "../PluginProcessor.h"

struct Control::PluginListListener : juce::ChangeListener {
    explicit PluginListListener(Control& c) : owner(c) {}

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
    setTooltip("Monitor Volume (Click to toggle)");
}

void Control::VolumeKnob::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds().toFloat();
    const float radius = std::min(bounds.getWidth(), bounds.getHeight()) * 0.5f - 2.0f;
    const float cx = bounds.getCentreX();
    const float cy = bounds.getCentreY();

    const float val = static_cast<float>(getValue());

    // Background
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

    // Outline
    g.setColour(juce::Colours::grey);
    g.drawEllipse(cx - radius, cy - radius, radius * 2.0f, radius * 2.0f, 1.0f);

    constexpr float startAngle = -juce::MathConstants<float>::pi * 0.75f;
    const float endAngle = startAngle + (val * juce::MathConstants<float>::pi * 1.5f);

    // Fill Arc
    if (val > 0.0f) {
        juce::Path arc;
        arc.addCentredArc(cx, cy, radius * 0.7f, radius * 0.7f, 0.0f, startAngle, endAngle, true);
        g.setColour(juce::Colours::white.withAlpha(0.8f));
        g.strokePath(arc, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved));
    }

    // Pointer
    const float px = cx + std::sin(endAngle) * radius * 0.7f;
    const float py = cy - std::cos(endAngle) * radius * 0.7f;

    g.setColour(val > 0.0f ? juce::Colours::white : juce::Colours::grey.withAlpha(0.5f));
    g.drawLine(cx, cy, px, py, 2.0f);
}

void Control::VolumeKnob::mouseDown(const juce::MouseEvent& e) {
    Slider::mouseDown(e);
    wasDragged = false;
}

void Control::VolumeKnob::mouseDrag(const juce::MouseEvent& e) {
    Slider::mouseDrag(e);
    if (e.mouseWasDraggedSinceMouseDown()) {
        wasDragged = true;
    }
}

void Control::VolumeKnob::mouseUp(const juce::MouseEvent& e) {
    Slider::mouseUp(e);
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

    toggleRollType.setButtonText(processor.settings.isUseRollStft ? "STFT" : "CQT");
    toggleRollType.onClick = [this] {
        processor.settings.isUseRollStft = !processor.settings.isUseRollStft;
        toggleRollType.setButtonText(processor.settings.isUseRollStft ? "STFT" : "CQT");
        if (onVisibilityChanged) onVisibilityChanged();
    };

    toggleOrientation.setButtonText(processor.settings.isRollHorizontal ? "Flip" : "Flop");
    toggleOrientation.onClick = [this] {
        processor.settings.isRollHorizontal = !processor.settings.isRollHorizontal;
        toggleOrientation.setButtonText(processor.settings.isRollHorizontal ? "Flip" : "Flop");
        if (onVisibilityChanged) onVisibilityChanged();
    };

    setupToggleButton(toggleForrest, processor.settings.isShowForrest);
    toggleForrest.onClick = [this] {
        processor.settings.isShowForrest = toggleForrest.getToggleState();
    };

    sliderEar.setValue(processor.settings.earVolume, juce::NotificationType::dontSendNotification);
    sliderEar.onValueChange = [this] {
        processor.settings.earVolume = static_cast<float>(sliderEar.getValue());
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

    buttonPlugs.setButtonText("Plugs");
    buttonPlugs.onClick = [this] {
        showPlugsMenu();
    };

    buttonPlug.setButtonText("Plug");
    buttonPlug.onClick = [this] {
        processor.showExternalPluginEditor();
    };

    comboPresets.setTextWhenNothingSelected("Presets...");
    comboPresets.onChange = [this] {
        const int id = comboPresets.getSelectedId();
        if (id == 0) return;

        std::unique_ptr<juce::XmlElement> xml;
        juce::File newPresetFile;
        juce::String newPresetName;

        if (id == 1) {
            newPresetName = "";
            const juce::File factoryDefaultFile(
                juce::File(__FILE__).getParentDirectory().getParentDirectory().getChildFile("factory-settings.xml")
            );
            xml = juce::XmlDocument::parse(factoryDefaultFile);
            if (xml != nullptr) xml->setTagName(getSettingsTagName());
        } else if (id == 2) {
            const auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).
                getChildFile("Pitchenga");
            newPresetFile = appDataDir.getChildFile("presets").getChildFile("user-default.xml");
            newPresetName = "User Default";

            if (newPresetFile.existsAsFile()) {
                xml = juce::XmlDocument::parse(newPresetFile);
            } else {
                // Fallback to factory if missing, but keep User Default context for Saving
                const juce::File factoryDefaultFile(
                    juce::File(__FILE__).getParentDirectory().getParentDirectory().getChildFile("factory-settings.xml")
                );
                xml = juce::XmlDocument::parse(factoryDefaultFile);
                if (xml != nullptr) xml->setTagName(getSettingsTagName());
            }
        } else if (id > 3) {
            const size_t index = static_cast<size_t>(id - 4);
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

    // Restore selection by name
    if (processor.settings.currentPresetName.isNotEmpty()) {
        const auto presetName = processor.settings.currentPresetName;
        if (presetName == "User Default") {
            comboPresets.setSelectedId(2, juce::NotificationType::dontSendNotification);
            const auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).
                getChildFile("Pitchenga");
            const auto presetsDir = appDataDir.getChildFile("presets");
            currentPresetFile = presetsDir.getChildFile("user-default.xml");
        } else if (presetName == "Factory Default") {
            comboPresets.setSelectedId(1, juce::NotificationType::dontSendNotification);
            currentPresetFile = juce::File();
        } else {
            for (size_t i = 0; i < presets.size(); ++i) {
                if (presets[i].getFileNameWithoutExtension() == presetName) {
                    // Item IDs for general presets start at 4
                    comboPresets.setSelectedId(static_cast<int>(i) + 4, juce::NotificationType::dontSendNotification);
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

    buttonSave.setButtonText("Save");
    buttonSave.onClick = [this] {
        if (currentPresetFile == juce::File()) {
            return;
        }

        juce::AlertWindow::showOkCancelBox(
            juce::MessageBoxIconType::QuestionIcon,
            saveConfirmTitle,
            saveConfirmMessage.replace("{NAME}", comboPresets.getText()),
            "Save",
            "Cancel",
            nullptr,
            juce::ModalCallbackFunction::create([this](int result) {
                if (result != 0) {
                    saveCurrentPreset();
                }
            })
        );
    };

    buttonSaveAs.setButtonText("Save As");
    buttonSaveAs.onClick = [this] {
        const juce::String currentName = comboPresets.getText();
        juce::String suggestedName = currentName;

        if (currentName == ""
            || currentName == "Presets..."
            || currentName == "Factory Default"
            || currentName == "User Default"
        ) {
            suggestedName = "user-default";
        }

        chooser = std::make_unique<juce::FileChooser>(
            "Select where to save the settings...",
            juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("Pitchenga").
            getChildFile("presets").getChildFile(suggestedName),
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
                    if (result.getParentDirectory().createDirectory() && xml.writeTo(result)) {
                        currentPresetFile = result;
                        processor.settings.currentPresetName = result.getFileNameWithoutExtension();
                        refreshPresets();
                        updateButtonStates();
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

        juce::AlertWindow::showOkCancelBox(
            juce::MessageBoxIconType::QuestionIcon,
            deleteConfirmTitle,
            deleteConfirmMessage.replace("{NAME}", comboPresets.getText()),
            "Delete",
            "Cancel",
            nullptr,
            juce::ModalCallbackFunction::create([this](int result) {
                if (result != 0) {
                    deleteCurrentPreset();
                }
            })
        );
    };

    addAndMakeVisible(toggleNeedle);
    addAndMakeVisible(toggleEye);
    addAndMakeVisible(toggleRoll);
    addAndMakeVisible(toggleIsFreezeRoll);

    addAndMakeVisible(sliderEar);

    addAndMakeVisible(toggleTweak);
    addAndMakeVisible(comboPresets);
    addAndMakeVisible(buttonSave);

    addAndMakeVisible(tweakPanel);
    tweakPanel.addAndMakeVisible(buttonPlugs);
    tweakPanel.addAndMakeVisible(buttonPlug);
    tweakPanel.addAndMakeVisible(toggleCapture);
    tweakPanel.addAndMakeVisible(toggleLayoutPivot);
    tweakPanel.addAndMakeVisible(toggleStrobe);
    tweakPanel.addAndMakeVisible(toggleRollType);
    tweakPanel.addAndMakeVisible(toggleOrientation);
    tweakPanel.addAndMakeVisible(toggleSmoke);
    tweakPanel.addAndMakeVisible(toggleForrest);
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
    for (int i = 0; i < types.size(); ++i) {
        if (types[i].isInstrument) {
            menu.addItem(id, types[i].name);
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
        if (currentPresetFile.getFileName() == "user-default.xml") {
            comboPresets.setText("User Default", juce::NotificationType::dontSendNotification);
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
    if (selectedId == 2) {
        // Delete User Default and flip to Factory Default
        if (currentPresetFile.deleteFile()) {
            currentPresetFile = juce::File();
            processor.settings.currentPresetName = "";
            comboPresets.setSelectedId(1, juce::NotificationType::sendNotification);
            refreshPresets();
        }
    } else if (selectedId > 3) {
        // Delete general preset
        if (currentPresetFile.deleteFile()) {
            currentPresetFile = juce::File();
            processor.settings.currentPresetName = "";
            comboPresets.setSelectedId(1, juce::NotificationType::sendNotification);
            refreshPresets();
        }
    }
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
    toggleLayoutPivot.setToggleState(processor.settings.isLayoutHorizontal, juce::NotificationType::dontSendNotification);
    toggleIsFreezeRoll.setToggleState(processor.settings.isFreezeRoll, juce::NotificationType::dontSendNotification);

    toggleRollType.setButtonText(processor.settings.isUseRollStft ? "STFT" : "CQT");
    toggleOrientation.setButtonText(processor.settings.isRollHorizontal ? "Flip" : "Flop");
    toggleStrobe.setToggleState(processor.settings.isShowStrobe, juce::NotificationType::dontSendNotification);
    toggleSmoke.setToggleState(processor.settings.isShowSmoke, juce::NotificationType::dontSendNotification);
    toggleForrest.setToggleState(processor.settings.isShowForrest, juce::NotificationType::dontSendNotification);

    sliderEar.setValue(processor.settings.earVolume, juce::NotificationType::dontSendNotification);
    toggleCapture.setToggleState(processor.settings.isCaptureEnabled, juce::NotificationType::dontSendNotification);
    toggleTweak.setToggleState(processor.settings.isShowTweakPanel, juce::NotificationType::dontSendNotification);

    updateButtonStates();
}

void Control::updateButtonStates() {
    const bool isStandalone = processor.wrapperType == juce::AudioProcessor::wrapperType_Standalone;
    buttonPlugs.setVisible(isStandalone);
    buttonPlug.setVisible(isStandalone);
    sliderEar.setVisible(isStandalone);
    toggleCapture.setVisible(isStandalone);

    const bool rollActive = processor.settings.isShowRoll;
    toggleIsFreezeRoll.setVisible(rollActive);
    toggleRollType.setVisible(rollActive);
    toggleStrobe.setVisible(processor.settings.isShowNeedle);
    toggleLayoutPivot.setEnabled(rollActive && processor.settings.isShowEye);
    toggleOrientation.setVisible(rollActive);
    toggleSmoke.setVisible(rollActive);
    toggleForrest.setVisible(rollActive);

    buttonPlug.setEnabled(processor.isExternalPluginLoaded());

    tweakPanel.setVisible(processor.settings.isShowTweakPanel);

    // Disable Save and Delete buttons for Factory Default (ID 1) and nothing selected (ID 0)
    const int selectedId = comboPresets.getSelectedId();
    buttonSave.setEnabled(selectedId > 1);
    buttonDelete.setEnabled(selectedId != 1 && selectedId != 0);

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

    if (processor.wrapperType == juce::AudioProcessor::wrapperType_Standalone) {
        const int sliderWidth = rowHeight + 10;
        sliderEar.setBounds(topRow.removeFromLeft(sliderWidth).reduced(2));
    }

    // Position save, presets and tweak from the right
    // buttonSave first from right makes it the rightmost
    positionButtonRight(buttonSave, topRow);
    const int comboWidth = 140;
    comboPresets.setBounds(topRow.removeFromRight(comboWidth).reduced(2));
    positionButtonRight(toggleTweak, topRow);

    positionButtonRight(toggleIsFreezeRoll, topRow);

    topRow.removeFromRight(8);
    buildTimestampLabel.setBounds(topRow);

    if (processor.settings.isShowTweakPanel) {
        tweakPanel.setBounds(bounds); // Use the entire remaining bounds (the bottom row)

        auto panelBounds = tweakPanel.getLocalBounds();
        positionButton(buttonPlugs, panelBounds);
        positionButton(buttonPlug, panelBounds);
        positionButton(toggleCapture, panelBounds);

        positionButtonRight(buttonSaveAs, panelBounds);
        positionButtonRight(buttonDelete, panelBounds);

        // Gap between save buttons and the moved roll control buttons
        panelBounds.removeFromRight(16);

        positionButtonRight(toggleForrest, panelBounds);
        positionButtonRight(toggleSmoke, panelBounds);
        positionButtonRight(toggleOrientation, panelBounds);
        positionButtonRight(toggleRollType, panelBounds);

        // Gap between Roll type and Strobe/Pivot group
        panelBounds.removeFromRight(16);
        positionButtonRight(toggleStrobe, panelBounds);
        panelBounds.removeFromRight(16);
        positionButtonRight(toggleLayoutPivot, panelBounds);
    }
}

void Control::refreshPresets() {
    comboPresets.clear(juce::NotificationType::dontSendNotification);
    comboPresets.addItem("Factory Default", 1);

    // User Default is always available now, falling back to Factory if file is missing
    comboPresets.addItem("User Default", 2);

    comboPresets.addSeparator();

    presets.clear();
    const auto appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("Pitchenga");
    const auto presetsDir = appDataDir.getChildFile("presets");
    if (presetsDir.exists()) {
        juce::Array<juce::File> files;
        presetsDir.findChildFiles(files, juce::File::findFiles, false, "*.xml");

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
            for (size_t i = 0; i < presets.size(); ++i) {
                if (presets[i].getFileNameWithoutExtension() == activeNameNoExt) {
                    comboPresets.setSelectedId(static_cast<int>(i) + 4, juce::NotificationType::dontSendNotification);
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
    xml.setAttribute("isRollHorizontal", isRollHorizontal);
    xml.setAttribute("isLayoutHorizontal", isLayoutHorizontal);

    xml.setAttribute("earVolume", earVolume);
    xml.setAttribute("isCaptureEnabled", isCaptureEnabled);
    xml.setAttribute("isShowTweakPanel", isShowTweakPanel);

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
    isRollHorizontal = xml.getBoolAttribute("isRollHorizontal", isRollHorizontal);
    isLayoutHorizontal = xml.getBoolAttribute("isLayoutHorizontal", isLayoutHorizontal);

    earVolume = static_cast<float>(xml.getDoubleAttribute("earVolume", earVolume));
    isCaptureEnabled = xml.getBoolAttribute("isCaptureEnabled", isCaptureEnabled);
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
    currentPresetName = xml.getStringAttribute("currentPresetName", currentPresetName);

    return true;
}
