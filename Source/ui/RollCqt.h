#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../PluginProcessor.h"
#include "../Math/Cqt.h"
#include "../math/Analyzers.h"

class RollCqt : public juce::Component
{
public:
    RollCqt(PitchengaAudioProcessor&);
    bool expand();
    void paint(juce::Graphics&) override;
    void resized() override;
    void updateResults(const std::vector<double>& results);
    void paintForrest(juce::Graphics& graphics) const;

    void rebuildFrame() { buildFrame(); repaint(); }

    void setEngine(const Cqt* e) { engine = e; }
    static constexpr int getPreferredHeight() { return 619; }

    static float getLabelAreaHeight();

private:
    void buildFrame();
    void paintFrame(juce::Graphics& graphics) const;

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

    //fixme: Implement speed
    const float smokeSpeedPxPerFrame = 1.0f;
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
};