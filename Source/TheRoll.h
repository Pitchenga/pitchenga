#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "Stft.h"

class TheRoll : public juce::Component {
public:
    explicit TheRoll(PitchengaAudioProcessor& proc);
    ~TheRoll() override = default;

    void updateResults(const std::vector<SpectralPeak>& peaks);

    void paint(juce::Graphics& graphics) override;
    void resized() override;

    bool enableDynamicStemWidth = true;

private:
    PitchengaAudioProcessor& processor;

    std::vector<SpectralPeak> activePeaks;

    juce::Image steamImage;
    int steamScrollOffset = 0;
    static constexpr float steamSpeedPxPerFrame = 1.0f;

    juce::Image cachedFrame;

    // Standard piano range mapping
    static constexpr float minMidiNote = 21.0f; // A0
    static constexpr float maxMidiNote = 108.0f; // C8

    static float freqToMidi(float freq);
    float frequencyToX(float frequencyHz, float width) const;

    void paintFrame();
    void paintFrame(juce::Graphics& graphics) const;
    void paintLabel(juce::Graphics& graphics, float labelHeight, float maxTextWidth, int midiNote, float targetCenter, float startY, juce::Colour baseColor) const;
    void pumpSteam();
    void paintSteam(const juce::Graphics& graphics) const;
    void paintPeaks(juce::Graphics& graphics) const;

    static juce::Font getLabelFont();
    static float getLabelAreaHeight();
    static juce::String getNoteName(int midiNote);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TheRoll)
};