#pragma once

#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <complex>
#include <algorithm>
#include <cmath>
#include "../Util.h"

namespace sevagh {

/**
 * Port of sevagh/pitch-detection Mpm algorithm to JUCE.
 * https://github.com/sevagh/pitch-detection
 */
template <typename T>
class PitchDetector {
public:
    explicit PitchDetector(int bufferSize)
        : numFftPoints(bufferSize),
          fftOrder(static_cast<int>(std::log2(bufferSize))),
          fft(fftOrder),
          fftBuffer(static_cast<size_t>(numFftPoints)),
          nsdfBuffer(static_cast<size_t>(numFftPoints))
    {
    }
    
    T getPitch(const std::vector<T>& audioBuffer, int sampleRate) {
        computeNsdf(audioBuffer);

        std::vector<int> maxPositions = peakPicking();
        if (maxPositions.empty()) {
            return static_cast<T>(-1.0);
        }

        std::vector<std::pair<T, T>> estimates;
        T highestAmplitude = static_cast<T>(-1e30);

        for (int i : maxPositions) {
            highestAmplitude = std::max(highestAmplitude, static_cast<T>(nsdfBuffer[static_cast<size_t>(i)]));
            if (nsdfBuffer[static_cast<size_t>(i)] > MpmSmallCutoff) {
                auto x = parabolicInterpolation(i);
                estimates.push_back(x);
                highestAmplitude = std::max(highestAmplitude, x.second);
            }
        }

        if (estimates.empty()) {
            return static_cast<T>(-1.0);
        }

        T actualCutoff = MpmCutoff * highestAmplitude;
        T period = static_cast<T>(0.0);

        for (auto i : estimates) {
            if (i.second >= actualCutoff) {
                period = i.first;
                break;
            }
        }

        if (period <= static_cast<T>(0.0)) {
            return static_cast<T>(-1.0);
        }
        
        T pitchEstimate = static_cast<T>(sampleRate) / period;
        
        if (pitchEstimate > MpmLowerPitchCutoff) {
            return pitchEstimate;
        }
        
        return static_cast<T>(-1.0);
    }

private:
    int numFftPoints;
    int fftOrder;
    juce::dsp::FFT fft;
    std::vector<juce::dsp::Complex<float>> fftBuffer;
    std::vector<float> nsdfBuffer;

    void computeNsdf(const std::vector<T>& audioBuffer) {
        const size_t size = static_cast<size_t>(numFftPoints);
        
        // Autocorrelation using Fft
        for (size_t i = 0; i < size; ++i) {
            fftBuffer[i] = { i < audioBuffer.size() ? static_cast<float>(audioBuffer[i]) : 0.0f, 0.0f };
        }
        fft.perform(fftBuffer.data(), fftBuffer.data(), false);

        for (size_t i = 0; i < size; ++i) {
            float real = fftBuffer[i].real;
            float imag = fftBuffer[i].imag;
            float magSq = real * real + imag * imag;
            fftBuffer[i] = { magSq, 0.0f };
        }
        fft.perform(fftBuffer.data(), fftBuffer.data(), true);

        // Square Sum m[k]
        std::vector<float> squareSums(size, 0.0f);
        float totalSumSquares = 0.0f;
        for (size_t i = 0; i < size; ++i) {
            float val = i < audioBuffer.size() ? static_cast<float>(audioBuffer[i]) : 0.0f;
            totalSumSquares += val * val;
        }

        squareSums[0] = 2.0f * totalSumSquares;
        for (size_t k = 1; k < size; ++k) {
            float valPrevious = (k - 1) < audioBuffer.size() ? static_cast<float>(audioBuffer[k - 1]) : 0.0f;
            float valEnd = (size - k) < audioBuffer.size() ? static_cast<float>(audioBuffer[size - k]) : 0.0f;
            squareSums[k] = squareSums[k - 1] - valPrevious * valPrevious - valEnd * valEnd;
        }

        // Nsdf = 2 * r[k] / m[k]
        float fftScaleFactor = 1.0f / static_cast<float>(numFftPoints);
        for (size_t k = 0; k < size; ++k) {
            float realCorrelation = fftBuffer[k].real * fftScaleFactor;
            if (squareSums[k] > 1e-6f) {
                nsdfBuffer[k] = 2.0f * realCorrelation / squareSums[k];
            } else {
                nsdfBuffer[k] = 0.0f;
            }
        }
    }

    std::vector<int> peakPicking() {
        std::vector<int> maxPositions;
        const int size = static_cast<int>(nsdfBuffer.size());
        const int searchRange = size / 2;

        for (int i = 1; i < searchRange - 1; ++i) {
            if (nsdfBuffer[static_cast<size_t>(i)] > nsdfBuffer[static_cast<size_t>(i - 1)] && 
                nsdfBuffer[static_cast<size_t>(i)] >= nsdfBuffer[static_cast<size_t>(i + 1)]) {
                if (nsdfBuffer[static_cast<size_t>(i)] > 0.0f) {
                    maxPositions.push_back(i);
                }
            }
        }
        return maxPositions;
    }

    std::pair<T, T> parabolicInterpolation(int peakIndex) {
        T x = static_cast<T>(peakIndex);
        int size = static_cast<int>(nsdfBuffer.size());

        if (peakIndex < 1 || peakIndex >= size - 1) {
             return { x, static_cast<T>(nsdfBuffer[static_cast<size_t>(peakIndex)]) };
        }

        T sampleBefore = static_cast<T>(nsdfBuffer[static_cast<size_t>(peakIndex - 1)]);
        T samplePeak = static_cast<T>(nsdfBuffer[static_cast<size_t>(peakIndex)]);
        T sampleAfter = static_cast<T>(nsdfBuffer[static_cast<size_t>(peakIndex + 1)]);

        T denominator = sampleBefore + sampleAfter - 2.0f * samplePeak;
        if (std::abs(denominator) < static_cast<T>(1e-10)) {
            return { x, samplePeak };
        }
        
        T delta = (sampleBefore - sampleAfter) / (static_cast<T>(2.0) * denominator);
        return { x + delta, samplePeak - (sampleBefore - sampleAfter) * delta / static_cast<T>(4.0) };
    }

    static constexpr T MpmCutoff = static_cast<T>(0.93);
    static constexpr T MpmSmallCutoff = static_cast<T>(0.3);
    static constexpr T MpmLowerPitchCutoff = static_cast<T>(80.0);
};

} // namespace sevagh
