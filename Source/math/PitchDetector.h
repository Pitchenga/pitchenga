#pragma once

#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <complex>
#include <algorithm>

/**
 * Port of sevagh/pitch-detection MPM algorithm to JUCE.
 * https://github.com/sevagh/pitch-detection
 */
namespace sevagh {

template <typename T>
class PitchDetector {
public:
    explicit PitchDetector(int bufferSize)
        : nfft(bufferSize),
          order(static_cast<int>(std::log2(bufferSize))),
          fft(order),
          fftBuffer(static_cast<size_t>(nfft)),
          nsdf(static_cast<size_t>(nfft))
    {
    }
    
    T getPitch(const std::vector<T>& audioBuffer, int sampleRate) {
        computeAcorr(audioBuffer);

        std::vector<int> maxPositions = peakPicking();
        std::vector<std::pair<T, T>> estimates;

        T highestAmplitude = -1e30f; // Use a very small number

        for (int i : maxPositions) {
            highestAmplitude = std::max(highestAmplitude, static_cast<T>(nsdf[static_cast<size_t>(i)]));
            if (nsdf[static_cast<size_t>(i)] > MPM_SMALL_CUTOFF) {
                auto x = parabolicInterpolation(i);
                estimates.push_back(x);
                highestAmplitude = std::max(highestAmplitude, x.second);
            }
        }

        if (estimates.empty())
            return -1.0;

        T actualCutoff = MPM_CUTOFF * highestAmplitude;
        T period = 0.0;

        for (auto i : estimates) {
            if (i.second >= actualCutoff) {
                period = i.first;
                break;
            }
        }

        if (period <= 0.0) return -1.0;
        
        T pitchEstimate = static_cast<T>(sampleRate) / period;
        return (pitchEstimate > MPM_LOWER_PITCH_CUTOFF) ? pitchEstimate : -1.0;
    }

private:
    int nfft;
    int order;
    juce::dsp::FFT fft;
    std::vector<std::complex<float>> fftBuffer;
    std::vector<float> nsdf;

    void computeAcorr(const std::vector<T>& audioBuffer) {
        for (size_t i = 0; i < static_cast<size_t>(nfft); ++i) {
            if (i < audioBuffer.size())
                fftBuffer[i] = { static_cast<float>(audioBuffer[i]), 0.0f };
            else
                fftBuffer[i] = { 0.0f, 0.0f };
        }

        fft.perform(fftBuffer.data(), fftBuffer.data(), false);

        float scale = 1.0f / static_cast<float>(nfft);
        for (size_t i = 0; i < static_cast<size_t>(nfft); ++i) {
            fftBuffer[i] = (fftBuffer[i] * std::conj(fftBuffer[i])) * scale;
        }

        fft.perform(fftBuffer.data(), fftBuffer.data(), true);

        for (size_t i = 0; i < static_cast<size_t>(nfft); ++i) {
            nsdf[i] = fftBuffer[i].real();
        }
    }

    std::vector<int> peakPicking() {
        std::vector<int> maxPositions;
        int pos = 0;
        int curMaxPos = 0;
        int size = nfft;

        while (pos < (size - 1) / 3 && nsdf[static_cast<size_t>(pos)] > 0)
            pos++;
        while (pos < size - 1 && nsdf[static_cast<size_t>(pos)] <= 0.0)
            pos++;

        if (pos == 0) pos = 1;

        while (pos < size - 1) {
            if (nsdf[static_cast<size_t>(pos)] > nsdf[static_cast<size_t>(pos - 1)] && 
                nsdf[static_cast<size_t>(pos)] >= nsdf[static_cast<size_t>(pos + 1)] &&
                (curMaxPos == 0 || nsdf[static_cast<size_t>(pos)] > nsdf[static_cast<size_t>(curMaxPos)])) {
                curMaxPos = pos;
            }
            pos++;
            if (pos < size - 1 && nsdf[static_cast<size_t>(pos)] <= 0) {
                if (curMaxPos > 0) {
                    maxPositions.push_back(curMaxPos);
                    curMaxPos = 0;
                }
                while (pos < size - 1 && nsdf[static_cast<size_t>(pos)] <= 0.0) {
                    pos++;
                }
            }
        }
        if (curMaxPos > 0) {
            maxPositions.push_back(curMaxPos);
        }
        return maxPositions;
    }

    std::pair<T, T> parabolicInterpolation(int x_) {
        T x = static_cast<T>(x_);
        int size = static_cast<int>(nsdf.size());

        if (x_ < 1) {
            int xAdjusted = (nsdf[0] <= nsdf[1]) ? 0 : 1;
            return { static_cast<T>(xAdjusted), static_cast<T>(nsdf[static_cast<size_t>(xAdjusted)]) };
        } else if (x_ >= size - 1) {
            int xAdjusted = (nsdf[static_cast<size_t>(size - 1)] <= nsdf[static_cast<size_t>(size - 2)]) ? size - 1 : size - 2;
            return { static_cast<T>(xAdjusted), static_cast<T>(nsdf[static_cast<size_t>(xAdjusted)]) };
        } else {
            T den = static_cast<T>(nsdf[static_cast<size_t>(x_ + 1)] + nsdf[static_cast<size_t>(x_ - 1)] - 2.0f * nsdf[static_cast<size_t>(x_)]);
            T delta = static_cast<T>(nsdf[static_cast<size_t>(x_ - 1)] - nsdf[static_cast<size_t>(x_ + 1)]);
            if (std::abs(den) < 1e-10)
                return { x, static_cast<T>(nsdf[static_cast<size_t>(x_)]) };
            
            return { x + delta / (2.0f * den),
                     static_cast<T>(nsdf[static_cast<size_t>(x_)]) - delta * delta / (8.0f * den) };
        }
    }

    static constexpr T MPM_CUTOFF = 0.93f;
    static constexpr T MPM_SMALL_CUTOFF = 0.5f;
    static constexpr T MPM_LOWER_PITCH_CUTOFF = 80.0f;
};

} // namespace sevagh
