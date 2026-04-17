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
        // Only attack instantly if it is a loud signal AND it is increasing
        if (currentFrame[i] >= 0.3 && currentFrame[i] > data[i]) {
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
    for (int i = 0; i < size; ++i) {
        for (int winIndex = 0; winIndex < windowSize; ++winIndex) {
            // Forward-looking asymmetric window
            int index = (i + winIndex + size) % size;
            window[static_cast<size_t>(winIndex)] = values[static_cast<size_t>(index)];
        }
        
        // Calculate median using nth_element (fast)
        std::vector<double> tempWindow = window; // Copy because nth_element mutates
        int halfWindow = windowSize / 2;
        std::nth_element(tempWindow.begin(), tempWindow.begin() + halfWindow, tempWindow.end());
        double median = tempWindow[static_cast<size_t>(halfWindow)];
        
        // MEDIAN_WEIGHT = 0.75
        filteredValues[static_cast<size_t>(i)] = values[static_cast<size_t>(i)] - 0.75 * median;
    }

    // EXACT JAVA PORT: normalizeViaMax (Includes the bizarre Java scaling logic)
    double newMax = 0.0;
    double origMax = 0.0;
    for (int i = 0; i < size; ++i) {
        newMax = std::max(newMax, filteredValues[i]);
        origMax = std::max(origMax, values[i]);
    }

    if (newMax > 0.0) {
        double factor = (origMax < 1.0) ? (origMax / newMax) : (1.0 / origMax);
        for (int i = 0; i < size; ++i) {
            filteredValues[i] *= factor;
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
    // EXACT JAVA PORT: Initialize with the fundamental energy (Adds the fundamental twice!)
    double dotProduct = cqBins[static_cast<size_t>(baseFreqBin)];
    int centerFreqBin = baseFreqBin - binsPerHalftoneHalf;

    for (int i = 1; i <= harmonicCount; ++i) {
        int harmonicBin = centerFreqBin + harmonicBinsIndexes[i - 1];

        if (harmonicBin >= 0 && harmonicBin < static_cast<int>(cqBins.size())) {
            double weight = 1.0 - 0.5 * (i - 1) * harmonicCountMinusOneInv; // HARMONIC_WEIGHT_FALLOFF = 0.5
            dotProduct += weight * cqBins[static_cast<size_t>(harmonicBin)];
        }
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
