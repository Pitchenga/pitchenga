#pragma once

#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <memory>

struct SpectralPeak {
    float frequencyHz = 0.0f;
    float magnitude = 0.0f;
};

struct StftBand {
    int windowSize = 0;
    int fftOrder = 0;
    int fftSize = 0;
    float smoothWeight = 0.4f;

    std::unique_ptr<juce::dsp::FFT> fft;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> window;

    std::vector<float> fftWorkspace;
    std::vector<float> magnitudes;
    std::vector<float> smoothedMagnitudes;
};

class Stft {
public:
    Stft();
    ~Stft() = default;

    void initialize(double sampleRateToUse);
    void processFrame(const std::vector<float>& timeDomainSignal);

    bool enablePeakExtraction = false;
    bool enablePsychoacousticTilt = true;
    bool enableTailKiller = false;
    bool enableTemporalSmoothing = true;

    const std::vector<SpectralPeak>& getPeaks() const { return finalPeaks; }

private:
    void performSTFT(const std::vector<float>& timeDomainSignal);
    void extractPeaks();
    void applyPsychoacousticTilt();
    void scaleForUi();
    void extractRawBins();

    double currentSampleRate = 44100.0;

    std::vector<StftBand> multiResolutionBands;
    std::vector<SpectralPeak> finalPeaks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Stft)
};