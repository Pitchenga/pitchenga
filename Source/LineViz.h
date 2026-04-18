#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "CqtEngine.h"

class LineViz : public juce::Component
{
public:
    LineViz(PitchengaAudioProcessor& p);
    void paint(juce::Graphics& g) override;
    void updateResults(const std::vector<double>& results);
    
    void setEngine(const CqtEngine* e) { engine = e; }

    static constexpr int getPreferredHeight() { return 100; }

private:
    PitchengaAudioProcessor& processor;
    const CqtEngine* engine = nullptr;
    std::vector<double> displayMagnitudes; 
    
    int currentTotalBins = 0;
    int currentBinsPerOctave = 0;
};
