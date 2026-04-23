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
    void paintBins(juce::Graphics& graphics) const;

    void setEngine(const Cqt* e) { engine = e; }
    static constexpr int getPreferredHeight() { return 619; }

    static juce::Font getLabelFont();
    static float getLabelAreaHeight();

    void pumpSteam();

private:
    void paintFrame();
    void paintFrame(juce::Graphics& graphics) const;

    static juce::String getNoteName(int midiNote);
    static void paintLabel(
        juce::Graphics& graphics,
        float labelHeight,
        float maxTextWidth,
        int i,
        float targetCenter,
        float startY,
        juce::Colour baseColor
    );

    const float steamThreshold = 0.0001f;
    double lastPumpTimeMs = 0.0;
    float subPixelAccumulator = 0.0f;
    int steamScrollOffset = 0;
    static constexpr float targetPixelsPerSecond = 48.0f;

    juce::Image steamImage;

    std::vector<double> accumulatedMagnitudes;
    std::vector<double> lastMagnitudes;

    void paintSteam(const juce::Graphics& graphics) const;

    PitchengaAudioProcessor& processor;
    const Cqt* engine = nullptr;
    std::vector<double> displayMagnitudes;

    int currentTotalBins = 0;
    int currentBinsPerOctave = 0;

    std::unique_ptr<ExpSmoother> smoother;
    size_t lastKnownSize = 0;

    juce::Image cachedFrame;
};