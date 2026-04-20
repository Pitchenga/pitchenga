#include "Stft.h"
#include <cmath>

Stft::Stft() {
    initialize(44100.0);
}

void Stft::initialize(const double sampleRateToUse) {
    currentSampleRate = sampleRateToUse > 0.0 ? sampleRateToUse : 44100.0;
    multiResolutionBands.clear();

    auto addResolutionBand = [this](const int windowSize, const int order, const float smoothWeight) {
        StftBand band;
        band.windowSize = windowSize;
        band.fftOrder = order;
        band.fftSize = 1 << order;
        band.smoothWeight = smoothWeight;

        band.fft = std::make_unique<juce::dsp::FFT>(order);
        band.window = std::make_unique<juce::dsp::WindowingFunction<float>>(
            static_cast<size_t>(windowSize),
            juce::dsp::WindowingFunction<float>::blackmanHarris
        );

        band.fftWorkspace.assign(static_cast<size_t>(band.fftSize * 2), 0.0f);
        band.magnitudes.assign(static_cast<size_t>(band.fftSize / 2), 0.0f);
        band.smoothedMagnitudes.assign(static_cast<size_t>(band.fftSize / 2), 0.0f);

        multiResolutionBands.push_back(std::move(band));
    };

    // All bands use Order 15 (32768 FFT Size = 1.34Hz precision) to guarantee continuous high-density peaks!
    // The window size shrinks for high frequencies to preserve transient timing via heavy zero-padding.
    // The smoothWeight is custom-tuned: low band is naturally sluggish so it gets light mathematical smoothing.
    // High band is extremely jittery so it gets heavy mathematical smoothing.
    addResolutionBand(16384, 15, 0.85f); // Low: 371ms window. Light smoothing to prevent over-smearing.
    addResolutionBand(4096,  15, 0.40f); // Mid: 92ms window. Medium smoothing.
    addResolutionBand(1024,  15, 0.15f); // High: 23ms window. Heavy smoothing to cure "neurotic" jerking.

    finalPeaks.reserve(1024);
}

void Stft::processFrame(const std::vector<float>& timeDomainSignal) {
    if (multiResolutionBands.empty() || timeDomainSignal.size() < static_cast<size_t>(multiResolutionBands.front().windowSize)) return;

    performSTFT(timeDomainSignal);

    if (enablePeakExtraction) {
        extractPeaks();
    } else {
        extractRawBins();
    }

    if (enablePsychoacousticTilt) {
        applyPsychoacousticTilt();
    }
    scaleForUi();
}

void Stft::performSTFT(const std::vector<float>& timeDomainSignal) {
    for (auto& band : multiResolutionBands) {
        const float smoothWeight = band.smoothWeight;
        const float smoothDecay = 1.0f - smoothWeight;

        // Zero-padding happens naturally because we only copy `windowSize` samples, leaving the rest of the workspace 0.0f
        const size_t offset = timeDomainSignal.size() - static_cast<size_t>(band.windowSize);
        std::copy(timeDomainSignal.begin() + static_cast<ptrdiff_t>(offset), timeDomainSignal.end(), band.fftWorkspace.begin());
        std::fill(band.fftWorkspace.begin() + band.windowSize, band.fftWorkspace.end(), 0.0f);

        band.window->multiplyWithWindowingTable(band.fftWorkspace.data(), static_cast<size_t>(band.windowSize));
        band.fft->performFrequencyOnlyForwardTransform(band.fftWorkspace.data());

        const float normalization = 1.0f / static_cast<float>(band.fftSize);

        for (int index = 0; index < band.fftSize / 2; ++index) {
            const float rawMag = band.fftWorkspace[static_cast<size_t>(index)] * normalization;
            if (enableTemporalSmoothing) {
                band.smoothedMagnitudes[static_cast<size_t>(index)] = smoothDecay * band.smoothedMagnitudes[static_cast<size_t>(index)] + smoothWeight * rawMag;
                band.magnitudes[static_cast<size_t>(index)] = band.smoothedMagnitudes[static_cast<size_t>(index)];
            } else {
                band.magnitudes[static_cast<size_t>(index)] = rawMag;
            }
        }
    }
}

void Stft::extractPeaks() {
    finalPeaks.clear();
    constexpr float crossoverLowHz = 250.0f;
    constexpr float crossoverHighHz = 2000.0f;

    for (size_t bandIndex = 0; bandIndex < multiResolutionBands.size(); ++bandIndex) {
        const auto& band = multiResolutionBands[bandIndex];
        const int halfSize = band.fftSize / 2;
        const float binResolution = static_cast<float>(currentSampleRate) / static_cast<float>(band.fftSize);

        for (int index = 1; index < halfSize - 1; ++index) {
            const float exactFrequency = static_cast<float>(index) * binResolution;

            // Route the bin directly to the responsible time-window band
            bool inActiveBand = false;
            if (bandIndex == 0 && exactFrequency < crossoverLowHz) inActiveBand = true;
            else if (bandIndex == 1 && exactFrequency >= crossoverLowHz && exactFrequency < crossoverHighHz) inActiveBand = true;
            else if (bandIndex == 2 && exactFrequency >= crossoverHighHz) inActiveBand = true;

            if (!inActiveBand) continue;

            const float magnitudeLeft = band.magnitudes[static_cast<size_t>(index - 1)];
            const float magnitudeCenter = band.magnitudes[static_cast<size_t>(index)];
            const float magnitudeRight = band.magnitudes[static_cast<size_t>(index + 1)];

            if (magnitudeCenter > magnitudeLeft && magnitudeCenter > magnitudeRight) {
                const float denominator = magnitudeLeft - 2.0f * magnitudeCenter + magnitudeRight;

                if (std::abs(denominator) > 1e-5f) {
                    const float fraction = 0.5f * (magnitudeLeft - magnitudeRight) / denominator;
                    const float interpolatedBin = static_cast<float>(index) + fraction;
                    const float interpolatedFrequency = interpolatedBin * binResolution;

                    const float exactMagnitude = magnitudeCenter - 0.25f * (magnitudeLeft - magnitudeRight) * fraction;

                    if (exactMagnitude > 1e-4f) {
                        finalPeaks.push_back({interpolatedFrequency, exactMagnitude});
                    }
                }
            }
        }
    }
}

void Stft::extractRawBins() {
    finalPeaks.clear();
    constexpr float crossoverLowHz = 250.0f;
    constexpr float crossoverHighHz = 2000.0f;

    for (size_t bandIndex = 0; bandIndex < multiResolutionBands.size(); ++bandIndex) {
        const auto& band = multiResolutionBands[bandIndex];
        const int halfSize = band.fftSize / 2;
        const float binResolution = static_cast<float>(currentSampleRate) / static_cast<float>(band.fftSize);

        for (int index = 1; index < halfSize; ++index) {
            const float exactFrequency = static_cast<float>(index) * binResolution;

            bool inActiveBand = false;
            if (bandIndex == 0 && exactFrequency < crossoverLowHz) inActiveBand = true;
            else if (bandIndex == 1 && exactFrequency >= crossoverLowHz && exactFrequency < crossoverHighHz) inActiveBand = true;
            else if (bandIndex == 2 && exactFrequency >= crossoverHighHz) inActiveBand = true;

            if (inActiveBand) {
                const float exactMagnitude = band.magnitudes[static_cast<size_t>(index)];
                if (exactMagnitude > 1e-6f) {
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

void Stft::scaleForUi() {
    constexpr float inputGain = 6.0f; // Window compensation and standard volume match
    constexpr float zeroAmplitudeDb = -90.0f;
    constexpr float zeroAmplitudeDbInv = 1.0f / zeroAmplitudeDb;

    for (auto& peak : finalPeaks) {
        const float linearAmplitude = peak.magnitude * inputGain;

        if (linearAmplitude <= 0.00003f) {
            peak.magnitude = 0.0f;
        } else {
            const float decibels = 20.0f * std::log10(linearAmplitude);
            float normalized = std::max(0.0f, 1.0f - decibels * zeroAmplitudeDbInv);

            if (enableTailKiller) {
                // Per-Bin Noise Gate (Tail Killer)
                constexpr float gateThreshold = 0.4f;
                if (normalized < gateThreshold) {
                    const float ratio = normalized / gateThreshold;
                    normalized = normalized * ratio * ratio;
                }
            }

            peak.magnitude = normalized;
        }
    }

    // Instantly wipe silent peaks so TheRoll doesn't waste UI processing power on 0.0f bars
    std::erase_if(finalPeaks, [](const SpectralPeak& p) { return p.magnitude <= 0.0f; });
}