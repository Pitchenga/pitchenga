#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

// Forward declare the processor to avoid circular includes
class PitchengaAudioProcessor;

class Control : public juce::Component {
public:
    // Pure data model for UI settings.
    // Handles its own XML parsing, but lives in the Processor to survive window closures.
    struct Settings {
        int lastUIWidth = 800;
        int lastUIHeight = 600;

        bool showLineViz = true;
        bool showCircleViz = true;
        bool showTunerViz = true;

        bool showSpectrum = true;
        bool showSpectrogram = true;

        float splitRatio = 0.5f;

        juce::XmlElement createXml() const;
        bool loadFromXml(const juce::XmlElement& xml);
    };

    explicit Control(PitchengaAudioProcessor& processorToUse);
    ~Control() override = default;

    void resized() override;
    void updateVisibilityFromState();

    // Callback so the Editor knows when a user clicked a toggle
    std::function<void()> onVisibilityChanged;

private:
    static void setupToggleButton(juce::TextButton& button, bool initialState);

    PitchengaAudioProcessor& audioProcessor;

    juce::TextButton toggleLineViz{"Line"};
    juce::TextButton toggleCircleViz{"Circle"};
    juce::TextButton toggleTunerViz{"Tuner"};

    juce::TextButton toggleSpectrum{"Spectrum"};
    juce::TextButton toggleSpectrogram{"Spectrogram"};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Control)
};