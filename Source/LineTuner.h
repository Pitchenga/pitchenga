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
    float freqToMidi (float freq) const;
    juce::String getNoteName (int midiNote) const;
    void paintLabel (juce::Graphics& graphics, int midiNote, float x, int stripY) const;

    // Range: Re1 to Re6
    float minMidi = 26.0f;
    float maxMidi = 86.0f;
    float currentMidi = -1.0f;

    juce::Image cachedGradient;

    static constexpr float dimmingFactor = 0.8f;
    static constexpr float tunerFontSize = 15.0f;
    static constexpr const char* tunerFontStyle = "Bold";

    juce::Font getTunerFont() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LineTuner)
};