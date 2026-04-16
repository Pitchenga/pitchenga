#include "Analyzers.h"
#include <cmath>
#include <algorithm>
#include <numeric>

// 1. ExpSmoother
ExpSmoother::ExpSmoother(int size, double currentWeight)
    : currentWeight(currentWeight), previousWeight(1.0 - currentWeight) {
    data.resize(size, 0.0);
}

const std::vector<double>& ExpSmoother::smooth(const std::vector<double>& currentFrame) {
    for (size_t i = 0; i < data.size(); ++i) {
        // Only attack instantly if it is a loud signal (> 0.5) AND it is increasing
        if (currentFrame[i] >= 0.5 && currentFrame[i] > data[i]) {
            data[i] = currentFrame[i];
        } else {
            // Smooth decays AND smooth low-level noise
            data[i] = previousWeight * data[i] + currentWeight * currentFrame[i];
        }
    }
    return data;
}

// 2. SpectralEqualizer
SpectralEqualizer::SpectralEqualizer(int size, int windowSize)
    : size(size), windowSize(windowSize) {
    filteredValues.resize(size, 0.0);
    window.resize(windowSize, 0.0);
}

const std::vector<double>& SpectralEqualizer::filter(const std::vector<double>& values) {
    const int halfWindow = windowSize / 2;
    for (int i = 0; i < size; ++i) {
        for (int winIndex = 0; winIndex < windowSize; ++winIndex) {
            // Center the window around i: range is [i - halfWindow, i + halfWindow]
            int index = (i + winIndex - halfWindow + size) % size;
            window[static_cast<size_t>(winIndex)] = values[static_cast<size_t>(index)];
        }
        
        std::nth_element(window.begin(), window.begin() + halfWindow, window.end());
        double median = window[static_cast<size_t>(halfWindow)];
        
        filteredValues[static_cast<size_t>(i)] = values[static_cast<size_t>(i)] - 0.75 * median; // MEDIAN_WEIGHT = 0.75
    }

    double newMax = 0.0;
    double origMax = 0.0;
    for (int i = 0; i < size; ++i) {
        newMax = std::max(newMax, filteredValues[i]);
        origMax = std::max(origMax, values[i]);
    }

    if (newMax > 0.0) {
        // Safety check to prevent extreme amplification of noise when signal is low
        double safeNewMax = std::max(newMax, 0.01);
        double factor = origMax / safeNewMax;
        
        for (int i = 0; i < size; ++i) {
            filteredValues[i] = std::clamp(filteredValues[i] * factor, 0.0, 1.0);
        }
    } else {
        std::fill(filteredValues.begin(), filteredValues.end(), 0.0);
    }

    return filteredValues;
}

// 3. HarmonicPatternPitchClassDetector
HarmonicPatternPitchClassDetector::HarmonicPatternPitchClassDetector(int binsPerOctave, int binsPerHalftone, int harmonicCount)
    : harmonicCount(harmonicCount), binsPerOctave(binsPerOctave), binsPerHalftoneHalf(binsPerHalftone / 2),
      harmonicCountMinusOneInv(1.0 / (harmonicCount - 1)) {
    
    harmonicBinsIndexes.resize(harmonicCount);
    for (int i = 0; i < harmonicCount; ++i) {
        harmonicBinsIndexes[i] = static_cast<int>(std::round(binsPerOctave * std::log2(i + 1.0))) + binsPerHalftoneHalf;
    }
}

double HarmonicPatternPitchClassDetector::extractHarmonics(const std::vector<double>& cqBins, int baseFreqBin) const {
    double dotProduct = 0.0;
    int centerFreqBin = baseFreqBin - binsPerHalftoneHalf;
    
    double totalPossibleWeight = 0.0;
    double accumulatedWeight = 0.0;

    for (int i = 1; i <= harmonicCount; ++i) {
        double weight = 1.0 - 0.5 * (i - 1) * harmonicCountMinusOneInv; // HARMONIC_WEIGHT_FALLOFF = 0.5
        totalPossibleWeight += weight;

        int harmonicBin = centerFreqBin + harmonicBinsIndexes[i - 1];
        if (harmonicBin >= 0 && harmonicBin < static_cast<int>(cqBins.size())) {
            dotProduct += weight * cqBins[static_cast<size_t>(harmonicBin)];
            accumulatedWeight += weight;
        }
    }

    // Compensation: If harmonics are truncated by the spectrum ceiling (common for high notes),
    // scale the result up to what it would have been with the full harmonic series.
    if (accumulatedWeight > 0.1) {
        dotProduct *= (totalPossibleWeight / accumulatedWeight);
    }

    return dotProduct;
}

const std::vector<double>& HarmonicPatternPitchClassDetector::detectPitchClasses(const std::vector<double>& cqBins) {
    int size = static_cast<int>(cqBins.size());
    if (static_cast<int>(harmonicBins.size()) != size) {
        harmonicBins.resize(size, 0.0);
    }

    for (int i = 0; i < size; ++i) {
        harmonicBins[i] = extractHarmonics(cqBins, i);
    }

    double harmonicSum = std::accumulate(harmonicBins.begin(), harmonicBins.end(), 0.0);
    double origSum = std::accumulate(cqBins.begin(), cqBins.end(), 0.0);

    if (harmonicSum > 0.0) {
        double normalizationFactor = origSum / harmonicSum;
        for (int i = 0; i < size; ++i) {
            harmonicBins[i] *= normalizationFactor;
        }
    }

    return harmonicBins;
}
