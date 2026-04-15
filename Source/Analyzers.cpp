#include "Analyzers.h"
#include <cmath>
#include <algorithm>
#include <numeric>

// 1. ExpSmoother
ExpSmoother::ExpSmoother(int size, double currentWeight)
    : currentWeight(currentWeight), previousWeight(1.0 - currentWeight) {
    data.resize(size, 0.0);
}

std::vector<double> ExpSmoother::smooth(const std::vector<double>& currentFrame) {
    for (size_t i = 0; i < data.size(); ++i) {
        if (currentFrame[i] < 0.5 || data[i] > currentFrame[i]) {
            data[i] = previousWeight * data[i] + currentWeight * currentFrame[i];
        } else {
            data[i] = currentFrame[i];
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

std::vector<double> SpectralEqualizer::filter(const std::vector<double>& values) {
    for (int i = 0; i < size; ++i) {
        for (int winIndex = 0; winIndex < windowSize; ++winIndex) {
            int index = (i + winIndex + size) % size;
            window[winIndex] = values[index];
        }
        
        std::vector<double> tempWindow = window;
        std::nth_element(tempWindow.begin(), tempWindow.begin() + windowSize / 2, tempWindow.end());
        double median = tempWindow[windowSize / 2];
        
        filteredValues[i] = values[i] - 0.75 * median; // MEDIAN_WEIGHT = 0.75
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
        double factor = origMax < 1.0 ? origMax / safeNewMax : 1.0 / origMax;
        
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
    double dotProduct = cqBins[baseFreqBin];
    int centerFreqBin = baseFreqBin - binsPerHalftoneHalf;
    
    for (int i = 1; i <= harmonicCount; ++i) {
        int harmonicBin = centerFreqBin + harmonicBinsIndexes[i - 1];
        if (harmonicBin < static_cast<int>(cqBins.size())) {
            double weight = 1.0 - 0.5 * (i - 1) * harmonicCountMinusOneInv; // HARMONIC_WEIGHT_FALLOFF = 0.5
            dotProduct += weight * cqBins[harmonicBin];
        }
    }
    return dotProduct;
}

std::vector<double> HarmonicPatternPitchClassDetector::detectPitchClasses(const std::vector<double>& cqBins) {
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
