#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_events/juce_events.h>
#include "PluginProcessor.h"
#include <array>
#include <vector>

// Ported Domain Models from Java logic - Renamed to match legacy solfège names
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
    void processFFT();
    static juce::Colour calculateColor (float velocity, float toneRatio);

    PitchengaAudioProcessor& audioProcessor;

    // FFT Setup (Order 12 = 4096 points)
    static constexpr int fftOrder = 12;
    static constexpr int fftSize = 1 << fftOrder;
    juce::dsp::FFT fft { fftOrder };
    juce::dsp::WindowingFunction<float> window { fftSize, juce::dsp::WindowingFunction<float>::hann };

    std::vector<float> fftData;
    std::vector<float> fifoWorkBuffer;

    // 60 bins (5 per semitone)
    static constexpr int numBins = 60;
    std::array<float, numBins> currentBins;
    std::array<float, numBins> smoothedBins;
    const float smoothingFactor = 0.2f;

    std::array<juce::Path, numBins> segmentPaths;

    // Ported Tone colors
    static const std::array<Pitch, 12> chromaticScale;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchengaAudioProcessorEditor)
};
