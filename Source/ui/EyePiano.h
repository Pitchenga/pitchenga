#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

class EyePiano : public juce::Component {
public:
    static constexpr int semitonesPerOctave = 12;
    static constexpr int binsPerSemitone = 9;
    static constexpr int totalFoldedBins = binsPerSemitone * semitonesPerOctave;

    EyePiano();

    void updateResults(const std::vector<double>& results);
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    std::vector<double> smoothedOctaveBins;
    
    // Cache for layout
    struct Lane {
        int semitone;
        bool isAccidental;
        float x;
        float width;
        juce::Colour color;
    };
    std::vector<Lane> lanes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EyePiano)
};
