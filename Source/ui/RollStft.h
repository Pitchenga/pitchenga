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

    void mouseMove(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

    //fixme: Does not seem to do anything?
    bool enableDynamicStemWidth = true;
    // bool enableDynamicStemWidth = false;

    static constexpr int minMidiNote = 22;
    static constexpr int maxMidiNote = 119;

private:
    PitchengaAudioProcessor& processor;

    std::vector<SpectralPeak> activePeaks;

    juce::Image smokeImage;
    int smokeScrollOffset = 0;
    static constexpr float smokeSpeedPxPerFrame = 1.0f;

    juce::Image cachedFrame;

    static float freqToMidi(float freq);
    static float frequencyToX(float frequencyHz, float width, float xOffset);
    void paintTooltip(juce::Graphics& graphics, int physicalWidth, int physicalHeight, juce::StringArray tooltipLines);
    void paintCrosshairs(
        juce::Graphics& graphics,
        int physicalWidth,
        int physicalHeight,
        bool isHorizontal,
        int logicalWidth,
        int plotHeight
    );

    void buildFrame();
    void paintHzAxis(
        juce::Graphics& graphics,
        float logicalWidth,
        const juce::Font& labelFont,
        float labelHeight,
        float totalHeight,
        float dbAxisWidth
    ) const;
    static void paintDbAxis(
        juce::Graphics& graphics,
        float plotHeight,
        bool isHorizontal,
        const juce::Font& labelFont,
        float dbAxisWidth
    );
    void paintLabel(
        juce::Graphics& graphics,
        float labelHeight,
        float maxTextWidth,
        int midiNote,
        float targetCenter,
        float startY,
        juce::Colour baseColor,
        bool isHorizontal
    ) const;
    static void paintHzLabel(
        juce::Graphics& graphics,
        float labelHeight,
        const juce::String& text,
        float targetCenter,
        float
        startY,
        juce::Colour color
    );
    void pumpSmoke();
    void paintSmoke(const juce::Graphics& graphics) const;
    void paintForrest(juce::Graphics& graphics) const;
    float getLabelAreaHeight() const;
    float getDbAxisWidth() const;

    void paintRawAxisLabels(juce::Graphics& graphics, float plotHeight, float logicalWidth);

    juce::Point<int> mousePosition{-1, -1};

    static inline const std::vector<float> hzValues = {
        20.0f,
        50.0f,
        100.0f,
        200.0f,
        500.0f,
        1000.0f,
        2000.0f,
        5000.0f,
        10000.0f
    };

    static inline const std::vector<int> dbValues = {
        -90,
        -80,
        -70,
        -60,
        -50,
        -40,
        -30,
        -20,
        -10
    };

    static constexpr float dbLabelFontSize = 10.0f;
    static constexpr float dbLabelMarginRight = 2.0f;
    static constexpr float dbLabelHeight = 10.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RollStft)
};
