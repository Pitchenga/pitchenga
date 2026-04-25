#pragma once

#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <memory>

struct SpectralPeak {
    float frequencyHz = 0.0f;
    float magnitude = 0.0f;
    float bandwidthHz = 1.0f;
    float rawMagnitude = 0.0f;
};

struct StftBand {
    int windowSize = 0;
    int fftOrder = 0;
    int fftSize = 0;

    std::unique_ptr<juce::dsp::FFT> fft;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> window;

    std::vector<float> fftWorkspace;
    std::vector<float> magnitudes;
};

class Stft {
public:
    Stft();
    ~Stft() = default;

    void initialize(double sampleRateToUse);
    void processFrame(const std::vector<float>& timeDomainSignal);

    bool isEnablePeakExtraction = true;
    // bool isEnablePeakExtraction = false;
    // bool isEnablePsychoacousticTilt = true;
    bool isEnablePsychoacousticTilt = false;
    bool isEnableTailKiller = true;
    // bool isEnableTailKiller = false;
    bool isEnableTemporalSmoothing = true;
    // bool isEnableTemporalSmoothing = false;

    // Gradiental Peak Extractor Factors
    float bassShrinkerExponent = 64.0f;    // High exponent to heavily pinch wide bass lobes
    float trebleShrinkerExponent = 0.01f;   // Low exponent to gently pinch thin treble lobes
    float peakExpanderVertical = 10.0f;     // Multiplicative makeup gain for the preserved summit

    [[nodiscard]] const std::vector<SpectralPeak>& getPeaks() const { return finalPeaks; }

private:
    void performStft(const std::vector<float>& timeDomainSignal);
    void calculateRawBands(const std::vector<float>& timeDomainSignal);
    void stitchResolutionBands();
    void extractPeaks();
    void applyProgressiveSmoothing();

    void applyPsychoacousticTilt();
    void scaleForUi();
    void extractRawBins();

    double currentSampleRate = 44100.0;

    std::vector<StftBand> multiResolutionBands;
    std::vector<SpectralPeak> finalPeaks;

    std::vector<float> stitchedMagnitudes;
    std::vector<float> smoothedMagnitudes;
    static constexpr int stitchedSize = 16384; // Reduced to 16k to halve CPU consumption

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Stft)
};