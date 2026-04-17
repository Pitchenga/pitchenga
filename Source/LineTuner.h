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

    // Defaults: La0 (A0 = 21.0f) to Mi4 (E4 = 64.0f)
    float minMidi = 21.0f;
    float maxMidi = 64.0f;
    float currentMidi = -1.0f;

    juce::Image cachedGradient;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LineTuner)
};