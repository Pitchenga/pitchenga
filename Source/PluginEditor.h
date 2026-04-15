#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_events/juce_events.h>
#include "PluginProcessor.h"
#include "CqtEngine.h"
#include "Analyzers.h"
#include <array>
#include <vector>
#include <memory>

enum class Tone { Do, Ra, Re, Me, Mi, Fa, Fi, So, Le, La, Te, Ti };

struct Pitch
{
    Tone tone;
    int number;
    double frequency;
    juce::Colour color;
};

class PitchengaAudioProcessorEditor : public juce::AudioProcessorEditor,
                                      private juce::Timer
{
public:
    explicit PitchengaAudioProcessorEditor (PitchengaAudioProcessor&);
    ~PitchengaAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void processCqt();
    static juce::Colour calculateColor (float velocity, float toneRatio);

    PitchengaAudioProcessor& audioProcessor;

    // CQT Engine and DSP Filters
    CqtEngine cqt;
    std::unique_ptr<HarmonicPatternPitchClassDetector> pcDetector;
    std::unique_ptr<SpectralEqualizer> spectralEqualizer;
    std::unique_ptr<ExpSmoother> octaveBinSmoother;

    std::vector<float> workBuffer;
    std::vector<std::complex<float>> cqtSpectrum;
    std::vector<double> amplitudeSpectrumDb;
    std::vector<double> octaveBins;
    std::vector<double> smoothedOctaveBins;

    // Rendering Constants
    static constexpr int semitonesPerOctave = 12;
    static constexpr int binsPerSemitone = 9;
    static constexpr int totalFoldedBins = binsPerSemitone * semitonesPerOctave;
    static constexpr double frequencyC0 = 16.35159783128741;

    std::array<juce::Path, totalFoldedBins> segmentPaths;
    juce::PathStrokeType strokeType { 0.5f };

    static const std::array<Pitch, 12> chromaticScale;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchengaAudioProcessorEditor)
};
