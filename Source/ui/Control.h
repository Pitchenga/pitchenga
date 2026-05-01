#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <functional>

// Forward declare the processor to avoid circular includes
class PitchengaAudioProcessor;

class Control : public juce::Component,
    juce::Timer {
public:
    class VolumeKnob : public juce::Slider {
    public:
        VolumeKnob();
        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent& e) override;
    private:
        bool wasDragged = false;
    };

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
        bool isShowStrobe = true;
        bool isShowSmoke = true;
        bool isShowForrest = false;
        bool isRollHorizontal = false;
        bool isLayoutHorizontal = false;

        float earVolume = 0.0f;
        bool isCaptureEnabled = false;
        bool isShowTweakPanel = false;

        juce::String externalPluginDescriptionXml;
        juce::String externalPluginStateBase64;
        bool isExternalPluginWindowOpen = false;

        float splitRatio = 0.3130223751068115f;
        juce::String currentPresetName;

        [[nodiscard]] bool isShowRollLabels() const {
            return isRollHorizontal || !(!isShowEye && isShowRoll && isShowNeedle);
        }

        [[nodiscard]] juce::XmlElement createXml() const;
        bool loadFromXml(const juce::XmlElement& xml);
    };

    explicit Control(PitchengaAudioProcessor& proc);
    ~Control() override;

    void resized() override;
    void updateVisibilityFromState();
    void refreshPresets();
    void showPlugsMenu();

    [[nodiscard]] float getPreferredHeight() const;

    // Callback so the Editor knows when the user clicked a toggle
    std::function<void()> onVisibilityChanged;

private:
    static inline const juce::String saveConfirmTitle = "Save Preset";
    static inline const juce::String saveConfirmMessage = "Are you sure you want to overwrite preset '{NAME}'?";
    static inline const juce::String deleteConfirmTitle = "Delete Preset";
    static inline const juce::String deleteConfirmMessage = "Are you sure you want to delete preset '{NAME}'?";

    void timerCallback() override;
    void saveCurrentPreset();
    void deleteCurrentPreset();
    static void setupToggleButton(juce::TextButton& button, bool initialState);
    void updateButtonStates();

    PitchengaAudioProcessor& processor;

    struct PluginListListener;
    std::unique_ptr<PluginListListener> listListener;
    bool isRescanning = false;

    juce::TextButton toggleNeedle{"Needle"};
    juce::TextButton toggleEye{"Eye"};
    juce::TextButton toggleRoll{"Roll"};
    juce::TextButton toggleStrobe{"Strobe"};
    juce::TextButton toggleRollType{"STFT"};
    juce::TextButton toggleOrientation{"Flip"};
    juce::TextButton toggleIsFreezeRoll{"Freeze"};
    juce::TextButton toggleLayoutPivot{"Pivot"};
    juce::TextButton toggleSmoke{"Smoke"};
    juce::TextButton toggleForrest{"Forest"};

    VolumeKnob sliderEar;
    juce::TextButton toggleCapture{"Capture"};
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
