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
        : maxBufferSize(bufferSize),
          numFftPoints(juce::nextPowerOfTwo(bufferSize * 2)),
          fftOrder(static_cast<int>(std::log2(numFftPoints))),
          fftEngine(fftOrder),
          fftBuffer(static_cast<size_t>(numFftPoints)),
          nsdfBuffer(static_cast<size_t>(maxBufferSize))
    {
    }
    
    T getPitch(const std::vector<T>& audioBuffer, int sampleRate) {
        const int actualSize = std::min(static_cast<int>(audioBuffer.size()), maxBufferSize);
        computeNsdf(audioBuffer, actualSize);

        const std::vector<int> maxPositions = peakPicking(actualSize);
        
        std::vector<std::pair<T, T>> estimates;
        T highestAmplitude = static_cast<T>(-1e30);

        for (int i : maxPositions) {
            const T val = static_cast<T>(nsdfBuffer[static_cast<size_t>(i)]);
            highestAmplitude = std::max(highestAmplitude, val);
            if (val > mpmSmallCutoff) {
                const auto x = parabolicInterpolation(i);
                estimates.push_back(x);
                highestAmplitude = std::max(highestAmplitude, x.second);
            }
        }

        static int logCounter = 0;
        if (++logCounter % 100 == 0) {
            float rms = 0.0f;
            for (auto v : audioBuffer) rms += static_cast<float>(v * v);
            rms = std::sqrt(rms / static_cast<float>(audioBuffer.size()));
            
            juce::String nsdfStart;
            for (int i = 0; i < std::min(10, actualSize); ++i) nsdfStart += juce::String(nsdfBuffer[static_cast<size_t>(i)]) + " ";

            Util::debug("PitchDetector - RMS=" + juce::String(rms) + 
                        ", highestAmplitude=" + juce::String(highestAmplitude) + 
                        ", maxPositions=" + juce::String(maxPositions.size()) +
                        ", estimates=" + juce::String(estimates.size()) +
                        ", nsdf[0..9]=" + nsdfStart);
        }

        if (estimates.empty()) {
            return static_cast<T>(-1.0);
        }

        const T actualCutoff = mpmCutoff * highestAmplitude;
        T period = static_cast<T>(0.0);

        for (const auto& i : estimates) {
            if (i.second >= actualCutoff) {
                period = i.first;
                break;
            }
        }

        if (period <= static_cast<T>(0.0)) {
            return static_cast<T>(-1.0);
        }
        
        const T pitchEstimate = static_cast<T>(sampleRate) / period;
        
        if (pitchEstimate > mpmLowerPitchCutoff) {
            return pitchEstimate;
        }
        
        return static_cast<T>(-1.0);
    }

private:
    int maxBufferSize;
    int numFftPoints;
    int fftOrder;
    juce::dsp::FFT fftEngine;
    std::vector<std::complex<float>> fftBuffer;
    std::vector<float> nsdfBuffer;

    void computeNsdf(const std::vector<T>& audioBuffer, int actualSize) {
        // Zero-pad to avoid circular correlation
        for (size_t i = 0; i < static_cast<size_t>(numFftPoints); ++i) {
            const float sampleValue = i < static_cast<size_t>(actualSize) ? static_cast<float>(audioBuffer[i]) : 0.0f;
            fftBuffer[i] = std::complex<float>(sampleValue, 0.0f);
        }
        
        fftEngine.perform(fftBuffer.data(), fftBuffer.data(), false);

        for (size_t i = 0; i < static_cast<size_t>(numFftPoints); ++i) {
            const float realPart = fftBuffer[i].real();
            const float imagPart = fftBuffer[i].imag();
            const float magnitudeSquared = realPart * realPart + imagPart * imagPart;
            fftBuffer[i] = std::complex<float>(magnitudeSquared, 0.0f);
        }
        
        fftEngine.perform(fftBuffer.data(), fftBuffer.data(), true);

        // Square Sum m[k]
        std::vector<float> squareSums(static_cast<size_t>(actualSize), 0.0f);
        float totalSumSquares = 0.0f;
        for (size_t i = 0; i < static_cast<size_t>(actualSize); ++i) {
            const float val = static_cast<float>(audioBuffer[i]);
            totalSumSquares += val * val;
        }

        squareSums[0] = 2.0f * totalSumSquares;
        for (size_t k = 1; k < static_cast<size_t>(actualSize); ++k) {
            const float valPrevious = static_cast<float>(audioBuffer[k - 1]);
            const float valEnd = static_cast<float>(audioBuffer[static_cast<size_t>(actualSize) - k]);
            squareSums[k] = std::max(0.0f, squareSums[k - 1] - valPrevious * valPrevious - valEnd * valEnd);
        }

        // Nsdf = 2 * r[k] / m[k]
        // JUCE inverse FFT scales by numFftPoints if using certain backends, but usually it DOES NOT scale.
        // If RMS=0.01 and N=4096, energy is ~0.4. squareSums[0] is ~0.8.
        // My logs showed fftBuffer[0].real() around 0.00012, which is way too small.
        // It means we need to check if we are using the correct scaling.
        // Actually, if RMS=0.01, sum x^2 = 0.0001 * 4096 = 0.4.
        // IFFT(FFT(x)^2) at lag 0 should be sum x^2.
        // If it's 0.00012, it means it was scaled by 1/M^2? Unlikely.
        // Let's re-calculate r[k] without any extra scaling first to see what we get.
        for (size_t k = 0; k < static_cast<size_t>(actualSize); ++k) {
            const float rawCorrelation = fftBuffer[k].real();
            if (squareSums[k] > 1e-10f) {
                nsdfBuffer[k] = 2.0f * rawCorrelation / squareSums[k];
            } else {
                nsdfBuffer[k] = 0.0f;
            }
        }
    }

    std::vector<int> peakPicking(int actualSize) {
        std::vector<int> maxPositions;
        int pos = 0;
        int curMaxPos = 0;
        const int size = actualSize;

        // Skip the initial positive region (the lag-0 peak)
        while (pos < size - 1 && nsdfBuffer[static_cast<size_t>(pos)] > 0.0f) {
            pos++;
        }
        // Move to the next positive region
        while (pos < size - 1 && nsdfBuffer[static_cast<size_t>(pos)] <= 0.0f) {
            pos++;
        }

        if (pos == 0) {
            pos = 1;
        }

        while (pos < size - 1) {
            if (nsdfBuffer[static_cast<size_t>(pos)] > nsdfBuffer[static_cast<size_t>(pos - 1)] && 
                nsdfBuffer[static_cast<size_t>(pos)] >= nsdfBuffer[static_cast<size_t>(pos + 1)]) {
                if (curMaxPos == 0 || nsdfBuffer[static_cast<size_t>(pos)] > nsdfBuffer[static_cast<size_t>(curMaxPos)]) {
                    curMaxPos = pos;
                }
            }
            pos++;
            if (pos < size - 1 && nsdfBuffer[static_cast<size_t>(pos)] <= 0.0f) {
                if (curMaxPos > 0) {
                    maxPositions.push_back(curMaxPos);
                    curMaxPos = 0;
                }
                while (pos < size - 1 && nsdfBuffer[static_cast<size_t>(pos)] <= 0.0f) {
                    pos++;
                }
            }
        }
        
        if (curMaxPos > 0) {
            maxPositions.push_back(curMaxPos);
        }
        
        return maxPositions;
    }

    std::pair<T, T> parabolicInterpolation(int peakIndex) {
        const T x = static_cast<T>(peakIndex);

        if (peakIndex < 1 || peakIndex >= static_cast<int>(nsdfBuffer.size()) - 1) {
             return { x, static_cast<T>(nsdfBuffer[static_cast<size_t>(peakIndex)]) };
        }

        const T sampleBefore = static_cast<T>(nsdfBuffer[static_cast<size_t>(peakIndex - 1)]);
        const T samplePeak = static_cast<T>(nsdfBuffer[static_cast<size_t>(peakIndex)]);
        const T sampleAfter = static_cast<T>(nsdfBuffer[static_cast<size_t>(peakIndex + 1)]);

        const T denominator = sampleBefore + sampleAfter - static_cast<T>(2.0) * samplePeak;
        if (std::abs(denominator) < static_cast<T>(1e-10)) {
            return { x, samplePeak };
        }
        
        const T delta = (sampleBefore - sampleAfter) / (static_cast<T>(2.0) * denominator);
        return { x + delta, samplePeak - (sampleBefore - sampleAfter) * delta / static_cast<T>(4.0) };
    }

    static constexpr T mpmCutoff = static_cast<T>(0.93);
    static constexpr T mpmSmallCutoff = static_cast<T>(0.5);
    static constexpr T mpmLowerPitchCutoff = static_cast<T>(50.0);
};

} // namespace sevagh
