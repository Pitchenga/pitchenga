#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

// Forward declare the processor to avoid circular includes
class PitchengaAudioProcessor;

class Control : public juce::Component {
public:
    // Data model for UI settings.
    // Handles its own XML parsing, but lives in the Processor to survive window closures.
    struct Settings {
        int lastUiWidth = 601;
        int lastUiHeight = 951;

        bool isShowRoll = true;
        bool isShowEye = true;
        bool isShowNeedle = true;

        bool isUseRollStft = true;
        bool isFreezeRoll = false;
        bool isShowSteam = true;
        bool isShowForrest = false;

        bool isEarEnabled = false;
        bool isShowTweakPanel = false;

        juce::String externalPluginDescriptionXml;
        juce::String externalPluginStateBase64;
        bool isExternalPluginWindowOpen = false;

        float splitRatio = 0.3130223751068115f;
        juce::String currentPresetName;

        [[nodiscard]] bool isShowRollLabels() const {
            return !(isShowRoll && isShowNeedle);
        }

        [[nodiscard]] juce::XmlElement createXml() const;
        bool loadFromXml(const juce::XmlElement& xml);
    };

    explicit Control(PitchengaAudioProcessor& proc);
    ~Control() override = default;

    void resized() override;
    void updateVisibilityFromState();
    void refreshPresets();

    [[nodiscard]] float getPreferredHeight() const;

    // Callback so the Editor knows when the user clicked a toggle
    std::function<void()> onVisibilityChanged;

private:
    static void setupToggleButton(juce::TextButton& button, bool initialState);
    void updateButtonStates();

    PitchengaAudioProcessor& processor;

    juce::TextButton toggleNeedle{"Needle"};
    juce::TextButton toggleEye{"Eye"};
    juce::TextButton toggleRoll{"Roll"};
    juce::TextButton toggleRollType{"STFT"};
    juce::TextButton toggleisFreezeRoll{"Freeze"};
    juce::TextButton toggleSteam{"Steam"};
    juce::TextButton toggleForrest{"Forest"};

    juce::TextButton toggleEar{"Ear"};
    juce::TextButton buttonPlugs{"Plugs"};
    juce::TextButton buttonPlug{"Plug"};

    juce::Label buildTimestampLabel;

    juce::TextButton toggleTweak{"Tweak"};
    juce::Component tweakPanel;
    juce::ComboBox comboPresets{"Presets"};
    std::vector<juce::File> presets;
    juce::File currentPresetFile;
    juce::TextButton buttonSave{"Save"};
    juce::TextButton buttonSaveAs{"Save As"};
    juce::TextButton buttonDelete{"Delete"};

    std::unique_ptr<juce::FileChooser> chooser;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Control)
};
