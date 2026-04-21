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
    addResolutionBand(8192, 15); // Low: 185ms window. Order 15 (32768) for 1.34Hz precision. CPU optimized.

    addResolutionBand(4096, 14); // Mid: 92ms window. Order 14 (16384) for 2.69Hz precision. CPU optimized.

    addResolutionBand(2048, 13); // High: 46ms window. Order 13 (8192) for 5.38Hz precision. CPU optimized.

    stitchedMagnitudes.assign(stitchedSize, 0.0f);
    smoothedMagnitudes.assign(stitchedSize, 0.0f);

    finalPeaks.reserve(1024);
}

void Stft::processFrame(const std::vector<float>& timeDomainSignal) {
    if (multiResolutionBands.empty() || timeDomainSignal.size() < static_cast<size_t>(multiResolutionBands.front().windowSize)) return;

    performStft(timeDomainSignal);

    // The Gradiental Peak Extractor (Unsharp Mask) physically sculpts the continuous spectrum inside performStft.
    // We must use extractRawBins to preserve the progressively carved visual width of these lobes.
    extractRawBins();

    if (enablePsychoacousticTilt) {
        applyPsychoacousticTilt();
    }
    scaleForUi();
}

void Stft::performStft(const std::vector<float>& timeDomainSignal) {
    calculateRawBands(timeDomainSignal);
    stitchResolutionBands();

    if (enablePeakExtraction) {
        sculptSpectrum();
    }

    applyProgressiveSmoothing();
}

void Stft::calculateRawBands(const std::vector<float>& timeDomainSignal) {
    // Calculate raw FFT magnitudes for all distinct bands
    for (auto& band : multiResolutionBands) {
        // Zero-padding happens naturally because we only copy windowSize samples, leaving the rest of the workspace 0.0f
        const size_t offset = timeDomainSignal.size() - static_cast<size_t>(band.windowSize);
        std::copy(timeDomainSignal.begin() + static_cast<ptrdiff_t>(offset), timeDomainSignal.end(), band.fftWorkspace.begin());
        std::fill(band.fftWorkspace.begin() + band.windowSize, band.fftWorkspace.end(), 0.0f);

        band.window->multiplyWithWindowingTable(band.fftWorkspace.data(), static_cast<size_t>(band.windowSize));
        band.fft->performFrequencyOnlyForwardTransform(band.fftWorkspace.data());

        const float normalization = 1.0f / static_cast<float>(band.fftSize);

        for (int i = 0; i < band.fftSize / 2; ++i) {
            band.magnitudes[static_cast<size_t>(i)] = band.fftWorkspace[static_cast<size_t>(i)] * normalization;
        }
    }
}

void Stft::stitchResolutionBands() {
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
    const float unifiedBinResolution = static_cast<float>(currentSampleRate) / 32768.0f;

    // Widened, logarithmic crossfades to completely mask Acoustic Group Delay tearing
    constexpr float lowMidFadeStart = 150.0f;
    constexpr float lowMidFadeEnd = 350.0f;
    constexpr float midHighFadeStart = 1000.0f;
    constexpr float midHighFadeEnd = 3000.0f;

    for (int i = 0; i < stitchedSize; ++i) {
        const float freq = static_cast<float>(i) * unifiedBinResolution;
        float magnitude = 0.0f;

        if (freq < lowMidFadeStart) {
            magnitude = getMagnitudeAtFreq(multiResolutionBands[0], freq, static_cast<float>(currentSampleRate));
        } else if (freq <= lowMidFadeEnd) {
            const float fadeMid = std::log10(freq / lowMidFadeStart) / std::log10(lowMidFadeEnd / lowMidFadeStart);
            const float fadeLow = 1.0f - fadeMid;
            magnitude = fadeLow * getMagnitudeAtFreq(multiResolutionBands[0], freq, static_cast<float>(currentSampleRate)) +
                        fadeMid * getMagnitudeAtFreq(multiResolutionBands[1], freq, static_cast<float>(currentSampleRate));
        } else if (freq < midHighFadeStart) {
            magnitude = getMagnitudeAtFreq(multiResolutionBands[1], freq, static_cast<float>(currentSampleRate));
        } else if (freq <= midHighFadeEnd) {
            const float fadeHigh = std::log10(freq / midHighFadeStart) / std::log10(midHighFadeEnd / midHighFadeStart);
            const float fadeMid = 1.0f - fadeHigh;
            magnitude = fadeMid * getMagnitudeAtFreq(multiResolutionBands[1], freq, static_cast<float>(currentSampleRate)) +
                        fadeHigh * getMagnitudeAtFreq(multiResolutionBands[2], freq, static_cast<float>(currentSampleRate));
        } else {
            magnitude = getMagnitudeAtFreq(multiResolutionBands[2], freq, static_cast<float>(currentSampleRate));
        }
        stitchedMagnitudes[static_cast<size_t>(i)] = magnitude;
    }
}

void Stft::sculptSpectrum() {
    // --- GRADIENTAL PEAK EXTRACTOR ---
    // Sculpt the spectrum to shrink wide lobes horizontally and expand peaks vertically
    std::vector<float> sharpenedMagnitudes(stitchedSize, 0.0f);
    const float unifiedBinResolution = static_cast<float>(currentSampleRate) / 32768.0f;

    constexpr float minFrequencyHz = 20.0f;
    constexpr float logMinFrequency = 1.301f;
    constexpr float logFrequencyRangeInv = 1.0f / 3.0f;

    // Logarithmic crossfade bounds to perfectly match stitchResolutionBands
    constexpr float lowMidFadeStart = 150.0f;
    constexpr float lowMidFadeEnd = 350.0f;
    constexpr float midHighFadeStart = 1000.0f;
    constexpr float midHighFadeEnd = 3000.0f;

    for (int i = 0; i < stitchedSize; ++i) {
        const float freq = static_cast<float>(i) * unifiedBinResolution;
        const float centerMagnitude = stitchedMagnitudes[static_cast<size_t>(i)];

        if (centerMagnitude < 1e-6f) continue;

        // The Blackman-Harris window has a main lobe width of ~8 bins natively.
        // We multiply the native half-width (4) by the zero-padding factor to find the exact shoulders.
        // All bands are now padded exactly 4x (e.g. 8192 window padded to 32768 FFT).
        // OUTDATED: Low band: 65536 / 8192 = 8 padding. 8 * 4 = 32 bins.
        // OUTDATED: High band: 65536 / 2048 = 32 padding. 32 * 4 = 128 bins.
        // OUTDATED: Mid band: 65536 / 4096 = 16 padding. 16 * 4 = 64 bins.

        // OUTDATED: int lobeHalfWidthBins = 16; // Low band natively 16 in the stitched array.
        // OUTDATED: if (freq >= 2000.0f) lobeHalfWidthBins = 64;      // High band mapped to unified array 4x larger relative res
        // OUTDATED: else if (freq >= 250.0f) lobeHalfWidthBins = 32;   // Mid band mapped to unified array 2x larger relative res

        float exactLobeHalfWidth = 16.0f;
        if (freq < lowMidFadeStart) {
            exactLobeHalfWidth = 16.0f;
        } else if (freq <= lowMidFadeEnd) {
            const float fade = std::log10(freq / lowMidFadeStart) / std::log10(lowMidFadeEnd / lowMidFadeStart);
            exactLobeHalfWidth = 16.0f * (1.0f - fade) + 32.0f * fade;
        } else if (freq < midHighFadeStart) {
            exactLobeHalfWidth = 32.0f;
        } else if (freq <= midHighFadeEnd) {
            const float fade = std::log10(freq / midHighFadeStart) / std::log10(midHighFadeEnd / midHighFadeStart);
            exactLobeHalfWidth = 32.0f * (1.0f - fade) + 64.0f * fade;
        } else {
            exactLobeHalfWidth = 64.0f;
        }
        int lobeHalfWidthBins = static_cast<int>(std::round(exactLobeHalfWidth));

        // Find the absolute local summit within this lobe's neighborhood
        float localMax = centerMagnitude;
        int startJ = std::max(0, i - lobeHalfWidthBins);
        int endJ = std::min(stitchedSize - 1, i + lobeHalfWidthBins);
        for (int j = startJ; j <= endJ; ++j) {
            if (stitchedMagnitudes[static_cast<size_t>(j)] > localMax) {
                localMax = stitchedMagnitudes[static_cast<size_t>(j)];
            }
        }

        // Logarithmic Progressive Shrinker:
        // Dynamically calculate the horizontal carving intensity to equalize the log-scale visual thickness.
        const float logFrequency = std::log10(std::max(minFrequencyHz, freq));
        const float logFrequencyNorm = std::min(1.0f, std::max(0.0f, (logFrequency - logMinFrequency) * logFrequencyRangeInv));
        const float dynamicExponent = bassShrinkerExponent - logFrequencyNorm * (bassShrinkerExponent - trebleShrinkerExponent);

        // Relative Peak Expander Math:
        // Calculate ratio compared to local summit (0.0 to 1.0)
        const float isolationRatio = centerMagnitude / localMax;

        // Raise ratio to exponent to pinch the shoulders, multiply back by localMax to restore true amplitude, then apply vertical makeup
        sharpenedMagnitudes[static_cast<size_t>(i)] = localMax * std::pow(isolationRatio, dynamicExponent) * peakExpanderVertical;
    }

    std::copy(sharpenedMagnitudes.begin(), sharpenedMagnitudes.end(), stitchedMagnitudes.begin());
}

void Stft::applyProgressiveSmoothing() {
    // Apply Unified Progressive Temporal Smoothing
    constexpr float minFrequencyHz = 20.0f;
    constexpr float logMinFrequency = 1.301f;
    constexpr float logFrequencyRangeInv = 1.0f / 3.0f;

    // Medium math smoothing for bass to cure jerkiness while avoiding sluggishness
    constexpr float bassSmoothWeight = 0.40f;
    // Extreme math smoothing for treble to lock down jitter
    constexpr float trebleSmoothWeight = 0.04f;

    const float unifiedBinResolution = static_cast<float>(currentSampleRate) / 65536.0f;

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

void Stft::extractRawBins() {
    finalPeaks.clear();
    const float binResolution = static_cast<float>(currentSampleRate) / 32768.0f;

    for (int i = 1; i < stitchedSize; ++i) {
        const float exactMagnitude = smoothedMagnitudes[static_cast<size_t>(i)];
        const float exactRawMagnitude = stitchedMagnitudes[static_cast<size_t>(i)];

        if (exactMagnitude > 1e-6f || exactRawMagnitude > 1e-6f) {
            const float exactFrequency = static_cast<float>(i) * binResolution;
            // For raw bins, pass the exact mathematical width so the UI can paint contiguous blocks
            finalPeaks.push_back({exactFrequency, exactMagnitude, binResolution, exactRawMagnitude});
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
            peak.rawMagnitude *= linearGain;
        }
    }
}

void Stft::scaleForUi() {
    constexpr float inputGain = 6.0f; // Window compensation and standard volume match
    constexpr float zeroAmplitudeDb = -90.0f;
    constexpr float zeroAmplitudeDbInv = 1.0f / zeroAmplitudeDb;
    constexpr float gateThreshold = 0.4f;

    for (auto& peak : finalPeaks) {
        const float linearAmplitude = peak.magnitude * inputGain;

        if (linearAmplitude <= 0.00003f) {
            peak.magnitude = 0.0f;
        } else {
            const float decibels = 20.0f * std::log10(linearAmplitude);
            float normalized = std::max(0.0f, 1.0f - decibels * zeroAmplitudeDbInv);

            if (enableTailKiller) {
                // Per-Bin Noise Gate (Tail Killer)
                if (normalized < gateThreshold) {
                    const float ratio = normalized / gateThreshold;
                    normalized = normalized * ratio * ratio;
                }
            }
            peak.magnitude = normalized;
        }

        const float rawLinearAmplitude = peak.rawMagnitude * inputGain;

        if (rawLinearAmplitude <= 0.00003f) {
            peak.rawMagnitude = 0.0f;
        } else {
            const float rawDecibels = 20.0f * std::log10(rawLinearAmplitude);
            float rawNormalized = std::max(0.0f, 1.0f - rawDecibels * zeroAmplitudeDbInv);

            if (enableTailKiller) {
                if (rawNormalized < gateThreshold) {
                    const float ratio = rawNormalized / gateThreshold;
                    rawNormalized = rawNormalized * ratio * ratio;
                }
            }
            peak.rawMagnitude = rawNormalized;
        }
    }

    // Instantly wipe silent peaks so TheRoll doesn't waste UI processing power on 0.0f bars
    std::erase_if(finalPeaks, [](const SpectralPeak& p) { return p.magnitude <= 0.0f && p.rawMagnitude <= 0.0f; });
}