#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <functional>
#include <atomic>

// Forward declare the processor to avoid circular includes
class PitchengaAudioProcessor;

class Control : public juce::Component,
    juce::Timer {
public:
    class VolumeKnob : public juce::Slider {
    public:
        VolumeKnob();
        void paint(juce::Graphics& graphics) override;
        void mouseDown(const juce::MouseEvent& event) override;
        void mouseDrag(const juce::MouseEvent& event) override;
        void mouseUp(const juce::MouseEvent& event) override;
    private:
        bool wasDragged = false;
    };

    // Data model for UI settings.
    // Handles its own XML parsing, but lives in the Processor to survive window closures.
    struct Settings {
        int lastUiWidth = 601;
        int lastUiHeight = 951;

        std::atomic<bool> isShowRoll = true;
        std::atomic<bool> isShowEye = true;
        std::atomic<bool> isShowNeedle = true;

        std::atomic<bool> isUseRollStft = true;
        bool isFreezeRoll = false;
        bool isShowStrobe = true;
        bool isShowSmoke = true;
        bool isShowForrest = false;
        bool isFlipRollHorizontal = false;
        bool isLayoutHorizontal = false;

        std::atomic<float> earVolumeLeft = 0.0f;
        std::atomic<float> earVolumeRight = 0.0f;
        std::atomic<bool> isCaptureEnabled = false;
        bool isShowTweakPanel = false;
        std::atomic<bool> isRawMode = false;
        std::atomic<bool> isLetterNotation = false;

        juce::String externalPluginDescriptionXml;
        juce::String externalPluginStateBase64;
        bool isExternalPluginWindowOpen = false;

        float splitRatio = 0.3130223751068115f;
        juce::String currentPresetName;

        [[nodiscard]] bool isShowRollLabels() const {
            return isFlipRollHorizontal || !(!isShowEye && isShowRoll && isShowNeedle);
        }

        [[nodiscard]] juce::XmlElement createXml() const;
        bool loadFromXml(const juce::XmlElement& xml);
    };

    explicit Control(PitchengaAudioProcessor& proc);
    ~Control() override;

    void timerCallback() override;
    void resized() override;
    void updateVisibilityFromState();
    void refreshPresets();
    void showPlugsMenu();

    [[nodiscard]] float getPreferredHeight() const;

    // Callback so the Editor knows when the user clicked a toggle
    std::function<void()> onVisibilityChanged;

    static inline const juce::String presetsDirectoryName = "presets";
    static inline const juce::String userDefaultPresetFileName = "user-default.xml";

private:
    static inline const juce::String saveConfirmTitle = "Save Preset";
    static inline const juce::String saveConfirmMessage = "Are you sure you want to save preset '{NAME}'?";
    static inline const juce::String deleteConfirmTitle = "Delete Preset";
    static inline const juce::String deleteConfirmMessage = "Are you sure you want to delete preset '{NAME}'?";

    static inline const juce::String labelNeedle = "Needle";
    static inline const juce::String labelEye = "Eye";
    static inline const juce::String labelRoll = "Roll";
    static inline const juce::String labelStrobe = "Strobe";
    static inline const juce::String labelRaw = "Raw";
    static inline const juce::String labelLetter = "Letter";
    static inline const juce::String labelSolfege = "Solfege";
    static inline const juce::String labelStft = "STFT";
    static inline const juce::String labelCqt = "CQT";
    static inline const juce::String labelFlip = "Flip";
    static inline const juce::String labelFlop = "Flop";
    static inline const juce::String labelFreeze = "Freeze";
    static inline const juce::String labelPivot = "Pivot";
    static inline const juce::String labelSmoke = "Smoke";
    static inline const juce::String labelForest = "Forest";
    static inline const juce::String labelCapture = "Capture";
    static inline const juce::String labelPlugs = "Plugs";
    static inline const juce::String labelPlug = "Plug";
    static inline const juce::String labelTweak = "Tweak";
    static inline const juce::String labelLoad = "Load";
    static inline const juce::String labelSave = "Save";
    static inline const juce::String labelSaveAs = "Save As";
    static inline const juce::String labelDelete = "Delete";

    static inline const juce::String userDefaultPresetName = "User Default";
    static inline const juce::String factoryDefaultPresetName = "Factory Default";
    static inline const juce::String presetsComboTextWhenNothingSelected = "Presets...";

    static constexpr int nonePresetId = 0;
    static constexpr int factoryDefaultPresetId = 1;
    static constexpr int userDefaultPresetId = 2;
    // 3 is separator
    static constexpr int customPresetsStartId = 4;

    void saveCurrentPreset();
    void deleteCurrentPreset();
    static void setupToggleButton(juce::TextButton& button, bool initialState);
    void updateButtonStates();

    PitchengaAudioProcessor& processor;

    struct PluginListListener;
    std::unique_ptr<PluginListListener> listListener;
    bool isRescanning = false;

    juce::TextButton toggleNeedle{labelNeedle};
    juce::TextButton toggleEye{labelEye};
    juce::TextButton toggleRoll{labelRoll};
    juce::TextButton toggleStrobe{labelStrobe};
    juce::TextButton toggleRaw{labelRaw};
    juce::TextButton toggleLetter{labelLetter};
    juce::TextButton toggleRollType{labelStft};
    juce::TextButton toggleFlipRoll{labelFlip};
    juce::TextButton toggleIsFreezeRoll{labelFreeze};
    juce::TextButton toggleLayoutPivot{labelPivot};
    juce::TextButton toggleSmoke{labelSmoke};
    juce::TextButton toggleForrest{labelForest};

    VolumeKnob knobEarLeft;
    VolumeKnob knobEarRight;
    juce::Label volumeLabelLeft;
    juce::Label volumeLabelRight;
    juce::TextButton toggleCapture{labelCapture};
    juce::TextButton buttonPlugs{labelPlugs};
    juce::TextButton buttonPlug{labelPlug};

    juce::Label buildTimestampLabel;

    juce::TextButton toggleTweak{labelTweak};
    juce::Component tweakPanel;
    juce::ComboBox comboPresets{presetsComboTextWhenNothingSelected};
    std::vector<juce::File> presets;
    juce::File currentPresetFile;
    juce::TextButton buttonLoad{labelLoad};
    juce::TextButton buttonSave{labelSave};
    juce::TextButton buttonSaveAs{labelSaveAs};
    juce::TextButton buttonDelete{labelDelete};

    std::unique_ptr<juce::FileChooser> chooser;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Control)
};
