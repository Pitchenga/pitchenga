#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../PluginProcessor.h"
#include "Cqt.h"
#include "../Analyzers.h"

class CqtRoll : public juce::Component
{
public:
    CqtRoll(PitchengaAudioProcessor&);
    bool expand();
    void paint(juce::Graphics&) override;
    void resized() override;
    void updateResults(const std::vector<double>& results);
    void paintBins(juce::Graphics& graphics) const;

    void setEngine(const Cqt* e) { engine = e; }
    static constexpr int getPreferredHeight() { return 619; }

    static juce::Font getLabelFont();
    static float getLabelAreaHeight();

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

    const float steamSpeedPxPerFrame = 1.0f;
    const float steamThreshold = 0.0001f;
    struct Steam {
        float x;
        float y;
        float width;
        juce::Colour color;
    };
    juce::Image steamImage;
    int steamScrollOffset = 0;

    void pumpSteam();
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