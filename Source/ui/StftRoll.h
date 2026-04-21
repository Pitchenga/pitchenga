#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../PluginProcessor.h"
#include "../math/Stft.h"

class StftRoll : public juce::Component {
public:
    explicit StftRoll(PitchengaAudioProcessor& proc);
    ~StftRoll() override = default;

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
    static float frequencyToX(float frequencyHz, float width);

    void paintFrame();
    void paintFrame(juce::Graphics& graphics) const;
    static void paintLabel(juce::Graphics& graphics, float labelHeight, float maxTextWidth, int midiNote, float targetCenter, float startY, juce::Colour baseColor);
    void pumpSteam();
    void paintSteam(const juce::Graphics& graphics) const;
    void paintPeaks(juce::Graphics& graphics) const;

    static juce::Font getLabelFont();
    static float getLabelAreaHeight();
    static juce::String getNoteName(int midiNote);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StftRoll)
};