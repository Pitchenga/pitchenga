#include "Stft.h"
#include <cmath>

Stft::Stft() {
    initialize(44100.0, 13);
}

void Stft::initialize(const double sampleRateToUse, const int fftOrderToUse) {
    currentSampleRate = sampleRateToUse > 0.0 ? sampleRateToUse : 44100.0;
    fftOrder = fftOrderToUse;
    fftSize = 1 << fftOrder;

    fft = std::make_unique<juce::dsp::FFT>(fftOrder);

    window = std::make_unique<juce::dsp::WindowingFunction<float>>(
        static_cast<size_t>(fftSize),
        juce::dsp::WindowingFunction<float>::blackmanHarris
    );

    fftWorkspace.assign(static_cast<size_t>(fftSize * 2), 0.0f);
    magnitudes.assign(static_cast<size_t>(fftSize / 2), 0.0f);

    finalPeaks.reserve(1024);
}

void Stft::processFrame(const std::vector<float>& timeDomainSignal) {
    if (timeDomainSignal.size() < static_cast<size_t>(fftSize)) return;

    performSTFT(timeDomainSignal);
    extractPeaks();
    applyPsychoacousticTilt();
}

void Stft::performSTFT(const std::vector<float>& timeDomainSignal) {
    std::copy(timeDomainSignal.begin(), timeDomainSignal.begin() + fftSize, fftWorkspace.begin());
    std::fill(fftWorkspace.begin() + fftSize, fftWorkspace.end(), 0.0f);

    window->multiplyWithWindowingTable(fftWorkspace.data(), static_cast<size_t>(fftSize));

    fft->performFrequencyOnlyForwardTransform(fftWorkspace.data());

    const float normalization = 1.0f / static_cast<float>(fftSize);
    for (int index = 0; index < fftSize / 2; ++index) {
        magnitudes[static_cast<size_t>(index)] = fftWorkspace[static_cast<size_t>(index)] * normalization;
    }
}

void Stft::extractPeaks() {
    finalPeaks.clear();
    const int halfSize = fftSize / 2;
    const float binResolution = static_cast<float>(currentSampleRate) / static_cast<float>(fftSize);

    for (int index = 1; index < halfSize - 1; ++index) {
        const float magnitudeLeft = magnitudes[static_cast<size_t>(index - 1)];
        const float magnitudeCenter = magnitudes[static_cast<size_t>(index)];
        const float magnitudeRight = magnitudes[static_cast<size_t>(index + 1)];

        if (magnitudeCenter > magnitudeLeft && magnitudeCenter > magnitudeRight) {
            const float denominator = magnitudeLeft - 2.0f * magnitudeCenter + magnitudeRight;

            if (std::abs(denominator) > 1e-5f) {
                const float fraction = 0.5f * (magnitudeLeft - magnitudeRight) / denominator;
                const float interpolatedBin = static_cast<float>(index) + fraction;
                const float exactFrequency = interpolatedBin * binResolution;

                const float exactMagnitude = magnitudeCenter - 0.25f * (magnitudeLeft - magnitudeRight) * fraction;

                if (exactMagnitude > 1e-4f) {
                    finalPeaks.push_back({exactFrequency, exactMagnitude});
                }
            }
        }
    }
}

void Stft::applyPsychoacousticTilt() {
    constexpr float anchorFrequency = 1000.0f;
    constexpr float tiltDbPerOctave = 3.0f;

    for (auto& peak : finalPeaks) {
        if (peak.frequencyHz > 0.0f) {
            const float octavesFromAnchor = std::log2(peak.frequencyHz / anchorFrequency);
            const float gainDb = octavesFromAnchor * tiltDbPerOctave;
            const float linearGain = std::pow(10.0f, gainDb / 20.0f);

            peak.magnitude *= linearGain;
        }
    }
}