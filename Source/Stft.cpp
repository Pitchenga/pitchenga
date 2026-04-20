#include "Stft.h"
#include <cmath>

Stft::Stft() {
    initialize(44100.0);
}

void Stft::initialize(const double sampleRateToUse) {
    currentSampleRate = sampleRateToUse > 0.0 ? sampleRateToUse : 44100.0;
    multiResolutionBands.clear();

    auto addResolutionBand = [this](const int windowSize, const int order) {
        StftBand band;
        band.windowSize = windowSize;
        band.fftOrder = order;
        band.fftSize = 1 << order;

        band.fft = std::make_unique<juce::dsp::FFT>(order);
        band.window = std::make_unique<juce::dsp::WindowingFunction<float>>(
            static_cast<size_t>(windowSize),
            juce::dsp::WindowingFunction<float>::blackmanHarris
        );

        band.fftWorkspace.assign(static_cast<size_t>(band.fftSize * 2), 0.0f);
        band.magnitudes.assign(static_cast<size_t>(band.fftSize / 2), 0.0f);

        multiResolutionBands.push_back(std::move(band));
    };

    // The window size shrinks for high frequencies to preserve transient timing via heavy zero-padding.
    // OUTDATED: Low: 185ms window. Order 16 (65536) for ultra-dense 0.67Hz sub-bass precision. Fast response.
    addResolutionBand(8192, 16);

    // OUTDATED: Mid: 92ms window. Order 15 (32768) for 1.34Hz precision. Medium response.
    addResolutionBand(4096, 15);

    // OUTDATED: High: 46ms window. Order 14 (16384) for 2.69Hz precision. Extreme smoothing to cure jitter.
    addResolutionBand(2048, 14);

    stitchedMagnitudes.assign(stitchedSize, 0.0f);
    smoothedMagnitudes.assign(stitchedSize, 0.0f);

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
    // 1. Calculate raw FFT magnitudes for all distinct bands
    for (auto& band : multiResolutionBands) {
        // Zero-padding happens naturally because we only copy windowSize samples, leaving the rest of the workspace 0.0f
        const size_t offset = timeDomainSignal.size() - static_cast<size_t>(band.windowSize);
        std::copy(timeDomainSignal.begin() + static_cast<ptrdiff_t>(offset), timeDomainSignal.end(), band.fftWorkspace.begin());
        std::fill(band.fftWorkspace.begin() + band.windowSize, band.fftWorkspace.end(), 0.0f);

        band.window->multiplyWithWindowingTable(band.fftWorkspace.data(), static_cast<size_t>(band.windowSize));
        band.fft->performFrequencyOnlyForwardTransform(band.fftWorkspace.data());

        const float normalization = 1.0f / static_cast<float>(band.fftSize);

        for (int index = 0; index < band.fftSize / 2; ++index) {
            band.magnitudes[static_cast<size_t>(index)] = band.fftWorkspace[static_cast<size_t>(index)] * normalization;
        }
    }

    // Interpolation helper
    auto getMagnitudeAtFreq = [](const StftBand& band, const float freq, const float sampleRate) {
        const float binRes = sampleRate / static_cast<float>(band.fftSize);
        const float exactIndex = freq / binRes;
        const int indexZero = static_cast<int>(exactIndex);
        const int indexOne = indexZero + 1;

        if (indexZero < 0 || indexOne >= band.fftSize / 2) return 0.0f;

        const float fraction = exactIndex - static_cast<float>(indexZero);
        return band.magnitudes[static_cast<size_t>(indexZero)] * (1.0f - fraction) + band.magnitudes[static_cast<size_t>(indexOne)] * fraction;
    };

    // Stitch and Cross-Fade the bands into a single unified high-resolution array
    const float unifiedBinResolution = static_cast<float>(currentSampleRate) / 65536.0f;

    for (int i = 0; i < stitchedSize; ++i) {
        const float freq = static_cast<float>(i) * unifiedBinResolution;
        float magnitude = 0.0f;

        if (freq < 200.0f) {
            magnitude = getMagnitudeAtFreq(multiResolutionBands[0], freq, static_cast<float>(currentSampleRate));
        } else if (freq <= 300.0f) {
            const float fadeMid = (freq - 200.0f) / 100.0f;
            const float fadeLow = 1.0f - fadeMid;
            magnitude = fadeLow * getMagnitudeAtFreq(multiResolutionBands[0], freq, static_cast<float>(currentSampleRate)) +
                        fadeMid * getMagnitudeAtFreq(multiResolutionBands[1], freq, static_cast<float>(currentSampleRate));
        } else if (freq < 1500.0f) {
            magnitude = getMagnitudeAtFreq(multiResolutionBands[1], freq, static_cast<float>(currentSampleRate));
        } else if (freq <= 2500.0f) {
            const float fadeHigh = (freq - 1500.0f) / 1000.0f;
            const float fadeMid = 1.0f - fadeHigh;
            magnitude = fadeMid * getMagnitudeAtFreq(multiResolutionBands[1], freq, static_cast<float>(currentSampleRate)) +
                        fadeHigh * getMagnitudeAtFreq(multiResolutionBands[2], freq, static_cast<float>(currentSampleRate));
        } else {
            magnitude = getMagnitudeAtFreq(multiResolutionBands[2], freq, static_cast<float>(currentSampleRate));
        }
        stitchedMagnitudes[static_cast<size_t>(i)] = magnitude;
    }

    // Apply Unified Progressive Temporal Smoothing
    constexpr float minFrequencyHz = 20.0f;
    constexpr float logMinFrequency = 1.301f;
    constexpr float logFrequencyRangeInv = 1.0f / 3.0f;

    constexpr float bassSmoothWeight = 0.40f;   // Medium math smoothing for bass to cure jerkiness while avoiding sluggishness
    constexpr float trebleSmoothWeight = 0.04f; // Extreme math smoothing for treble to lock down jitter

    for (int i = 0; i < stitchedSize; ++i) {
        const float freq = static_cast<float>(i) * unifiedBinResolution;
        float dynamicSmoothWeight = 1.0f;

        if (enableTemporalSmoothing) {
            // Logarithmic Progressive Smoother:
            // Calculates smooth weight continuously based on exact frequency. Eliminates all crossover tears.
            const float logFrequency = std::log10(std::max(minFrequencyHz, freq));
            // Maps log10(20) to log10(20000) -> 0.0 to 1.0
            const float logFrequencyNorm = std::min(1.0f, std::max(0.0f, (logFrequency - logMinFrequency) * logFrequencyRangeInv));

            dynamicSmoothWeight = bassSmoothWeight - logFrequencyNorm * (bassSmoothWeight - trebleSmoothWeight);
        }

        const float smoothDecay = 1.0f - dynamicSmoothWeight;
        smoothedMagnitudes[static_cast<size_t>(i)] = smoothDecay * smoothedMagnitudes[static_cast<size_t>(i)] + dynamicSmoothWeight * stitchedMagnitudes[static_cast<size_t>(i)];
    }
}

void Stft::extractPeaks() {
    finalPeaks.clear();
    const float binResolution = static_cast<float>(currentSampleRate) / 65536.0f;

    // OUTDATED: Route the bin directly to the responsible time-window band
    // (We now iterate entirely over the unified stitched array, eliminating the need to track individual bands).

    for (int index = 1; index < stitchedSize - 1; ++index) {
        const float magnitudeLeft = smoothedMagnitudes[static_cast<size_t>(index - 1)];
        const float magnitudeCenter = smoothedMagnitudes[static_cast<size_t>(index)];
        const float magnitudeRight = smoothedMagnitudes[static_cast<size_t>(index + 1)];

        if (magnitudeCenter > magnitudeLeft && magnitudeCenter > magnitudeRight) {
            const float denominator = magnitudeLeft - 2.0f * magnitudeCenter + magnitudeRight;

            if (std::abs(denominator) > 1e-5f) {
                const float fraction = 0.5f * (magnitudeLeft - magnitudeRight) / denominator;
                const float interpolatedBin = static_cast<float>(index) + fraction;
                const float interpolatedFrequency = interpolatedBin * binResolution;

                const float exactMagnitude = magnitudeCenter - 0.25f * (magnitudeLeft - magnitudeRight) * fraction;

                if (exactMagnitude > 1e-4f) {
                    // For interpolated peaks, bandwidth is technically zero, so we pass 0.0f to force a razor-thin visual stem
                    finalPeaks.push_back({interpolatedFrequency, exactMagnitude, 0.0f});
                }
            }
        }
    }
}

void Stft::extractRawBins() {
    finalPeaks.clear();
    const float binResolution = static_cast<float>(currentSampleRate) / 65536.0f;

    for (int index = 1; index < stitchedSize; ++index) {
        const float exactMagnitude = smoothedMagnitudes[static_cast<size_t>(index)];
        if (exactMagnitude > 1e-6f) {
            const float exactFrequency = static_cast<float>(index) * binResolution;
            // For raw bins, pass the exact mathematical width so the UI can paint contiguous blocks
            finalPeaks.push_back({exactFrequency, exactMagnitude, binResolution});
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