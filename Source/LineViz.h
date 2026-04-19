#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "CqtEngine.h"
#include "Analyzers.h"

class LineViz : public juce::Component
{
public:
    LineViz(PitchengaAudioProcessor&);
    bool expand();
    void paint(juce::Graphics&) override;
    void resized() override;
    void updateResults(const std::vector<double>& results);
    void paintBins(juce::Graphics& graphics) const;

    void setEngine(const CqtEngine* e) { engine = e; }
    static constexpr int getPreferredHeight() { return 620; }

private:
    void paintFrame();
    void paintFrame(juce::Graphics& graphics) const;

    const float bubblesSpeedPxPerFrame = 3.0f;
    const float bubbleThreshold = 0.2f;
    struct Bubble {
        float x;
        float y;
        float width;
        juce::Colour color;
    };
    std::vector<Bubble> bubbles;

    void advanceBubbles();
    void paintBubbles(juce::Graphics& graphics) const;

    PitchengaAudioProcessor& processor;
    const CqtEngine* engine = nullptr;
    std::vector<double> displayMagnitudes;

    int currentTotalBins = 0;
    int currentBinsPerOctave = 0;

    std::unique_ptr<ExpSmoother> smoother;
    size_t lastKnownSize = 0;

    juce::Image cachedFrame;
};