#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "CqtEngine.h"
#include "Analyzers.h"

class LineViz : public juce::Component
{
public:
    LineViz(PitchengaAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
    void updateResults(const std::vector<double>& results);
    
    void setEngine(const CqtEngine* e) { engine = e; }
    static constexpr int getPreferredHeight() { return 300; }

private:
    void paintFrame();
    void paintFrame(juce::Graphics& graphics) const;

    PitchengaAudioProcessor& processor;
    const CqtEngine* engine = nullptr;
    std::vector<double> displayMagnitudes;

    int currentTotalBins = 0;
    int currentBinsPerOctave = 0;

    std::unique_ptr<ExpSmoother> smoother;
    size_t lastKnownSize = 0;

    juce::Image cachedFrame;
};