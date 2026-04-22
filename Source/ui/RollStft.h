#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../PluginProcessor.h"
#include "../math/Stft.h"
#include "../math/Math.h"

class RollStft : public juce::Component {
public:
    explicit RollStft(PitchengaAudioProcessor& proc);
    ~RollStft() override = default;

    void updateResults(const std::vector<SpectralPeak>& peaks);
    void pumpSteam();

    void paint(juce::Graphics& graphics) override;
    void resized() override;

    bool enableDynamicStemWidth = true;

private:
    PitchengaAudioProcessor& processor;

    std::vector<SpectralPeak> activePeaks;

    juce::Image steamImage;
    int steamScrollOffset = 0;

    int64_t lastPumpSamples = 0;
    float subPixelAccumulator = 0.0f;
    static constexpr float targetPixelsPerSecond = 48.0f;

    juce::Image cachedFrame;

    static constexpr int minMidiNote = 12.0;
    static constexpr int maxMidiNote = 108.0;

    static float freqToMidi(float freq);
    static float frequencyToX(float frequencyHz, float width);

    void paintFrame();
    void paintFrame(juce::Graphics& graphics) const;
    static void paintLabel(juce::Graphics& graphics, float labelHeight, float maxTextWidth, int midiNote, float targetCenter, float startY, juce::Colour baseColor);

    void paintSteam(const juce::Graphics& graphics) const;
    void paintPeaks(juce::Graphics& graphics) const;

    static juce::Font getLabelFont();
    static float getLabelAreaHeight();
    static juce::String getNoteName(int midiNote);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RollStft)
};