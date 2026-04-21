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
        int lastUIWidth = 800;
        int lastUIHeight = 600;

        bool showRoll = true;
        bool showEye = true;
        bool showTuna = true;

        bool pauseRoll = false;
        bool showSteam = true;
        bool showForrest = true;

        float splitRatio = 0.5f;

        [[nodiscard]] juce::XmlElement createXml() const;
        bool loadFromXml(const juce::XmlElement& xml);
    };

    explicit Control(PitchengaAudioProcessor& processorToUse);
    ~Control() override = default;

    void resized() override;
    void updateVisibilityFromState();

    static float getPreferredHeight();

    // Callback so the Editor knows when the user clicked a toggle
    std::function<void()> onVisibilityChanged;

private:
    static void setupToggleButton(juce::TextButton& button, bool initialState);
    void updateButtonStates();

    PitchengaAudioProcessor& audioProcessor;

    juce::TextButton toggleTuna{"Tuna"};
    juce::TextButton toggleEye{"Eye"};
    juce::TextButton toggleRoll{"Roll"};
    juce::TextButton togglePauseRoll{"Pause"};
    juce::TextButton toggleSteam{"Steam"};
    juce::TextButton toggleForrest{"Forest"};
    juce::Label buildTimestampLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Control)
};