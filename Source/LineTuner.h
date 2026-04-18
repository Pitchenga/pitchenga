#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "ColorPalette.h"

class LineTuner : public juce::Component
{
public:
    LineTuner();

    void setPitchFrequency (float frequencyHz);
    void setRange (float minMidiNote, float maxMidiNote);

    void paint (juce::Graphics& graphics) override;
    void resized() override;

private:
    void updateCachedGradient();
    static float freqToMidi (float freq);
    static juce::String getNoteName (int midiNote);
    static void paintLabel (juce::Graphics& graphics, int midiNote, float x, float stripY);

    // Range: Re1 to Re6
    float minMidi = 26.0f;
    float maxMidi = 86.0f;
    float currentMidi = -1.0f;

    juce::Image cachedGradient;

    static constexpr float stripHeight = 16.0f;
    static constexpr float tickHeight = 5.0f;
    static constexpr float needleTriangleWidth = 12.0f;
    static constexpr float dimmingFactor = 0.9f;
    static constexpr float tunerFontSize = 15.0f;
    static constexpr auto tunerFontStyle = "Bold";

    static juce::Font getTunerFont();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LineTuner)
};