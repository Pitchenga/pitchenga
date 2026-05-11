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
    static inline const juce::String userDefaultPresetFileName = "Default.xml";

private:
    static inline const juce::String saveConfirmTitle = "Save Preset";
    static inline const juce::String saveConfirmMessage = "Are you sure you want to save preset '{NAME}'?";
    static inline const juce::String deleteConfirmTitle = "Delete Preset";
    static inline const juce::String deleteConfirmMessage = "Are you sure you want to delete preset '{NAME}'?";

    static inline const juce::String needle = "Needle";
    static inline const juce::String eye = "Eye";
    static inline const juce::String roll = "Roll";
    static inline const juce::String strobe = "Strobe";
    static inline const juce::String raw = "Raw";
    static inline const juce::String letter = "Letter";
    static inline const juce::String solfege = "Solfege";
    static inline const juce::String stft = "STFT";
    static inline const juce::String cqt = "CQT";
    static inline const juce::String flip = "Flip";
    static inline const juce::String flop = "Flop";
    static inline const juce::String freeze = "Freeze";
    static inline const juce::String pivot = "Pivot";
    static inline const juce::String smoke = "Smoke";
    static inline const juce::String forest = "Forest";
    static inline const juce::String mic = "Mic";
    static inline const juce::String capture = "Capture";
    static inline const juce::String plugs = "Plugs";
    static inline const juce::String plug = "Plug";
    static inline const juce::String tweak = "Tweak";
    static inline const juce::String save = "Save";
    static inline const juce::String saveAs = "Save As";
    static inline const juce::String deletePreset = "Delete";
    static inline const juce::String rename = "Rename";

    static inline const juce::String userDefaultPresetName = "Default";
    static inline const juce::String factoryDefaultPresetName = "Factory";
    static inline const juce::String presetsComboTextWhenNothingSelected = "Presets...";

    static constexpr int nonePresetId = 0;
    static constexpr int factoryDefaultPresetId = 1;
    static constexpr int userDefaultPresetId = 2;
    // 3 is separator
    static constexpr int customPresetsStartId = 4;

    void saveCurrentPreset();
    void deleteCurrentPreset();
    void renameCurrentPreset();

    struct NoEllipsisLookAndFeel : juce::LookAndFeel_V4 {
        NoEllipsisLookAndFeel();
        void drawButtonText(juce::Graphics& graphics, juce::TextButton& button, bool isMouseOverButton, bool isButtonDown) override;
        juce::Font getTextButtonFont(juce::TextButton& button, int buttonHeight) override;
    };

    NoEllipsisLookAndFeel noEllipsisLookAndFeel;

    void setupButton(juce::TextButton& button);
    void setupToggleButton(juce::TextButton& button, bool initialState);
    void updateButtonStates();

    PitchengaAudioProcessor& processor;

    struct PluginListListener;
    std::unique_ptr<PluginListListener> listListener;
    bool isRescanning = false;

    juce::TextButton toggleNeedle{needle};
    juce::TextButton toggleEye{eye};
    juce::TextButton toggleRoll{roll};
    juce::TextButton toggleStrobe{strobe};
    juce::TextButton toggleRaw{raw};
    juce::TextButton toggleLetter{letter};
    juce::TextButton toggleRollType{stft};
    juce::TextButton toggleFlipRoll{flip};
    juce::TextButton toggleIsFreezeRoll{freeze};
    juce::TextButton toggleLayoutPivot{pivot};
    juce::TextButton toggleSmoke{smoke};
    juce::TextButton toggleForrest{forest};

    VolumeKnob knobEarLeft;
    VolumeKnob knobEarRight;
    juce::Label micLabel;
    juce::Label volumeLabelLeft;
    juce::Label volumeLabelRight;
    juce::TextButton toggleCapture{capture};
    juce::TextButton buttonPlugs{plugs};
    juce::TextButton buttonPlug{plug};

    juce::Label buildTimestampLabel;

    juce::TextButton toggleTweak{tweak};
    juce::Component tweakPanel;
    juce::ComboBox comboPresets{presetsComboTextWhenNothingSelected};
    std::vector<juce::File> presets;
    juce::File currentPresetFile;
    juce::TextButton buttonSave{save};
    juce::TextButton buttonSaveAs{saveAs};
    juce::TextButton buttonDelete{deletePreset};
    juce::TextButton buttonRename{rename};

    std::unique_ptr<juce::FileChooser> chooser;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Control)
};
