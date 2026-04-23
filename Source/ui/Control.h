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
        int lastUIWidth = 601;
        int lastUIHeight = 951;

        bool showRoll = true;
        bool showEye = true;
        bool showNeedle = true;

        bool useStftRoll = true;
        bool freezeRoll = false;
        bool showSteam = true;
        bool showForrest = false;

        float splitRatio = 0.3130223751068115f;

        [[nodiscard]] juce::XmlElement createXml() const;
        bool loadFromXml(const juce::XmlElement& xml);
    };

    explicit Control(PitchengaAudioProcessor& processorToUse);
    ~Control() override = default;

    void resized() override;
    void updateVisibilityFromState();

    [[nodiscard]] float getPreferredHeight() const;

    // Callback so the Editor knows when the user clicked a toggle
    std::function<void()> onVisibilityChanged;

private:
    static void setupToggleButton(juce::TextButton& button, bool initialState);
    void updateButtonStates();

    PitchengaAudioProcessor& audioProcessor;

    juce::TextButton toggleNeedle{"Needle"};
    juce::TextButton toggleEye{"Eye"};
    juce::TextButton toggleRoll{"Roll"};
    juce::TextButton toggleRollType{"STFT"};
    juce::TextButton toggleFreezeRoll{"Freeze"};
    juce::TextButton toggleSteam{"Steam"};
    juce::TextButton toggleForrest{"Forest"};

    juce::Label buildTimestampLabel;

    juce::TextButton toggleTweak{"Tweak"};
    bool showTweakPanel = false;
    juce::Component tweakPanel;
    juce::TextButton buttonCopy{"Copy"};
    juce::TextButton buttonNuke{"Nuke"};


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Control)
};