#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>
#include "../Tone.h"

class Tuna : public juce::Component {
public:
    Tuna();

    void setPitchFrequency(float frequencyHz);
    void setRange(float minMidiNote, float maxMidiNote);

    void paint(juce::Graphics& graphics) override;
    void resized() override;

    static constexpr float stripHeight = 16.0f;
    static constexpr float tickHeight = 5.0f;
    // labelHeight and labelWidth names are intentionally swapped because the label is rendered sideways
    static float getLabelHeight();
    static float getLabelWidth();
    static float getPreferredHeight();

    static constexpr float needleTriangleWidth = 12.0f;
    static constexpr float tunaFontSize = 15.0f;
    static constexpr auto tunaFontStyle = "Bold";

private:
    void updateCachedLabels();
    static float freqToMidi(float freq);
    static juce::String getNoteName(int midiNote);
    static void paintLabel(juce::Graphics& graphics, int midiNote, float x, float stripY);

    // fixme: restore the Pitch enum and use Pitch references instead of hard-coded mini
    // fixme: Does not work below Mi2
    // Range: Ra2 to Mi6
    float minMidi = 39.0f;
    float maxMidi = 87.0f;
    float currentMidi = -1.0f;

    juce::Image cachedLabels;

    float strobePhase = 0.0f;
    float targetVelocity = 0.0f;
    float currentVelocity = 0.0f;
    static constexpr int strobeCycleWidth = 60;
    std::array<float, 60> strobeIntensities;

    int framesSinceSignalLost = 0;
    static constexpr int spinHoldFrames = 64;

    static juce::Font getLabelFont();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Tuna)
};