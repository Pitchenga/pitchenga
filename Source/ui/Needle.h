#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <array>

#include "RollStft.h"

class Needle : public juce::Component {
public:
    explicit Needle(PitchengaAudioProcessor& proc);

    void setPitchFrequency(float frequencyHz);
    void setRange(float minMidiNote, float maxMidiNote);

    void paint(juce::Graphics& graphics) override;
    void resized() override;
    void paintStrobeOverlay(juce::Graphics& graphics, float stripY, int width) const;
    void paintTunerNeedle(juce::Graphics& graphics, juce::Rectangle<float> bounds, float height) const;
    void paintLabelHighlight(juce::Graphics& graphics, juce::Rectangle<float> bounds, float height) const;

    static constexpr float stripHeight = 16.0f;
    static constexpr float tickHeight = 5.0f;
    // labelHeight and labelWidth names are intentionally swapped because the label is rendered sideways
    static float getPreferredHeight();

    static constexpr float needleTriangleWidth = 12.0f;

private:
    void buildFrame();
    static float freqToMidi(float freq);
    void paintLabel(juce::Graphics& graphics, int midiNote, float x, float stripY) const;

    // fixme: restore the Pitch enum and use Pitch references instead of hard-coded midi
    float minMidi = RollStft::minMidiNote;
    float maxMidi = RollStft::maxMidiNote;
    float currentMidi = -1.0f;

    juce::Image cachedLabels;
    juce::Image cachedGradient;

    float strobePhase = 0.0f;
    float targetVelocity = 0.0f;
    float currentVelocity = 0.0f;
    static constexpr int strobeCycleWidth = 60;
    std::array<float, 60> strobeIntensities{};

    int framesSinceSignalLost = 0;
    static constexpr int spinHoldFrames = 64;

    PitchengaAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Needle)
};
