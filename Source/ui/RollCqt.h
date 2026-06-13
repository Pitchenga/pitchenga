#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../PluginProcessor.h"
#include "../math/Cqt.h"
#include "../math/Analyzers.h"

class RollCqt : public juce::Component {
public:
    RollCqt(PitchengaAudioProcessor&);

    void updateResults(const std::vector<double>& results);
    bool expand();
    void paint(juce::Graphics& graphics) override;
    void resized() override;

    void mouseMove(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

    void paintForrest(juce::Graphics& graphics) const;

    void setEngine(const Cqt* e) { engine = e; }
    static constexpr int getPreferredHeight() { return 619; }

    static float getLabelAreaHeight();

private:
    void buildFrame();

    void paintTooltip(
        juce::Graphics& graphics,
        int physicalWidth,
        int physicalHeight,
        const juce::StringArray& tooltipLines
    ) const;

    void paintCrosshairs(
        juce::Graphics& graphics,
        int physicalWidth,
        int physicalHeight,
        bool isHorizontal,
        int logicalWidth,
        int plotHeight
    ) const;

    void paintLabel(
        juce::Graphics& graphics,
        float labelHeight,
        float maxTextWidth,
        int binIndex,
        float targetCenter,
        float startY,
        juce::Colour baseColor,
        bool isHorizontal
    ) const;

    const float smokeSpeedPxPerFrame = 2.0f;
    const float smokeThreshold = 0.0001f;

    struct Smoke {
        float x;
        float y;
        float width;
        juce::Colour color;
    };

    juce::Image smokeImage;
    int smokeScrollOffset = 0;

    void pumpSmoke();
    void paintSmoke(const juce::Graphics& graphics) const;

    PitchengaAudioProcessor& processor;
    const Cqt* engine = nullptr;
    std::vector<double> displayMagnitudes;

    int currentTotalBins = 0;
    int currentBinsPerOctave = 0;

    std::unique_ptr<ExpSmoother> smoother;
    size_t lastKnownSize = 0;

    juce::Image cachedFrame;
    juce::Point<int> mousePosition{-1, -1};
    juce::AffineTransform cachedHorizontalTransform;
    juce::AffineTransform cachedHorizontalTransformInverted;
};
