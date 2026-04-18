#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <array>
#include "ColorPalette.h"

class CircleVisualizer : public juce::Component
{
public:
    static constexpr int semitonesPerOctave = 12;
    static constexpr int binsPerSemitone = 9;
    static constexpr int totalFoldedBins = binsPerSemitone * semitonesPerOctave;

    CircleVisualizer();

    void updateResults (const std::vector<double>& results);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    static juce::Colour calculateColor (float velocity, float toneRatio);

    std::vector<double> smoothedOctaveBins;
    std::array<juce::Path, totalFoldedBins> segmentPaths;
    juce::PathStrokeType strokeType { 0.5f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CircleVisualizer)
};
