#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../PluginProcessor.h"
#include "../math/Stft.h"

class RollStft : public juce::Component {
public:
    explicit RollStft(PitchengaAudioProcessor& proc);
    ~RollStft() override = default;

    void updateResults(const std::vector<SpectralPeak>& peaks);

    void paint(juce::Graphics& graphics) override;
    void resized() override;

    //fixme: Does not seem to do anything?
    bool enableDynamicStemWidth = true;
    // bool enableDynamicStemWidth = false;

    static constexpr int minMidiNote = 22;
    static constexpr int maxMidiNote = 94;

private:
    PitchengaAudioProcessor& processor;

    std::vector<SpectralPeak> activePeaks;

    juce::Image smokeImage;
    int smokeScrollOffset = 0;
    static constexpr float smokeSpeedPxPerFrame = 1.0f;

    juce::Image cachedFrame;

    static float freqToMidi(float freq);
    static float frequencyToX(float frequencyHz, float width);

    void buildFrame();
    void paintFrame(juce::Graphics& graphics) const;
    void paintLabel(juce::Graphics& graphics, float labelHeight, float maxTextWidth, int midiNote, float targetCenter, float startY, juce::Colour baseColor, bool isHorizontal) const;
    void paintHzLabel(juce::Graphics& graphics, float labelHeight, float maxTextWidth, const juce::String& text, float targetCenter, float startY, juce::Colour color, bool isHorizontal) const;
    void paintTextLabel(juce::Graphics& graphics, float labelHeight, float maxTextWidth, const juce::String& text, float targetCenter, float startY, juce::Colour color, bool isHorizontal) const;
    void pumpSmoke();
    void paintSmoke(const juce::Graphics& graphics) const;
    void paintForrest(juce::Graphics& graphics) const;

    float getLabelAreaHeight() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RollStft)
};