#pragma once

#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <memory>

struct SpectralPeak {
    float frequencyHz = 0.0f;
    float magnitude = 0.0f;
};

class Stft {
public:
    Stft();
    ~Stft() = default;

    void initialize(double sampleRateToUse, int windowSizeToUse, int fftOrderToUse);
    void processFrame(const std::vector<float>& timeDomainSignal);

    bool enablePeakExtraction = false;
    bool enablePsychoacousticTilt = true;
    bool enableTailKiller = true;

    const std::vector<SpectralPeak>& getPeaks() const { return finalPeaks; }

private:
    void performSTFT(const std::vector<float>& timeDomainSignal);
    void extractPeaks();
    void applyPsychoacousticTilt();
    void scaleForUi();
    void extractRawBins();

    double currentSampleRate = 44100.0;
    int windowSize = 8192;
    int fftOrder = 13;
    int fftSize = 8192;

    std::unique_ptr<juce::dsp::FFT> fft;
    std::unique_ptr<juce::dsp::WindowingFunction<float>> window;

    std::vector<float> fftWorkspace;
    std::vector<float> magnitudes;
    std::vector<SpectralPeak> finalPeaks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Stft)
};