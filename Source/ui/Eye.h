#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <array>

// Forward declare the processor to avoid circular includes
class PitchengaAudioProcessor;

class Eye : public juce::Component {
public:
    static constexpr bool paintLogo = false;
    static constexpr int semitonesPerOctave = 12;
    static constexpr int binsPerSemitone = 9;
    static constexpr int totalFoldedBins = binsPerSemitone * semitonesPerOctave;

    explicit Eye(PitchengaAudioProcessor& processor);

    void updateResults(const std::vector<double>& results);

    void paint(juce::Graphics& g) override;
    void paintLabel(
        juce::Graphics& graphics,
        juce::Point<float> center,
        float baseRadius,
        float startRadius,
        int i,
        float sin,
        float cos
    ) const;
    void resized() override;
    static juce::Colour calculateColor(float velocity, float toneRatio);
    void paintBins(juce::Graphics& graphics) const;

private:
    void buildFrame(juce::Graphics& graphics) const;
    void paintFrame();
    void buildBins();

    PitchengaAudioProcessor& processor;

    std::vector<double> smoothedOctaveBins;
    std::array<juce::Path, totalFoldedBins> segmentPaths;
    juce::PathStrokeType strokeType{0.5f};

    juce::Image cachedFrame;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Eye)
};
