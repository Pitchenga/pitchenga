#pragma once

#include <vector>

// Asymmetric smoothing for punchy visual attack and slower decay.
class ExpSmoother {
public:
    ExpSmoother(size_t size, double weight);
    const std::vector<double>& smooth(const std::vector<double>& currentFrame);

private:
    std::vector<double> data;
    double currentWeight;
    double previousWeight;
};

// Uses a rolling median filter to shave off the noise floor and the "skirts" of spectral leakage.
class SpectralEqualizer {
public:
    SpectralEqualizer(size_t size, size_t windowSize);
    const std::vector<double>& filter(const std::vector<double>& values);

private:
    size_t size;
    size_t windowSize;
    std::vector<double> filteredValues;
    std::vector<double> window;
};

// Weights each bin by checking if its harmonic overtones exist, silencing non-harmonic noise.
class HarmonicPatternPitchClassDetector {
public:
    HarmonicPatternPitchClassDetector(int binsPerOctave, int binsPerSemitone, int harmonicCount = 10);
    const std::vector<double>& detectPitchClasses(const std::vector<double>& cqBins);

private:
    int harmonicCount;
    int binsPerSemitoneHalf;
    double harmonicCountMinusOneInv;

    std::vector<double> harmonicBins;
    std::vector<int> harmonicBinsIndexes;

    [[nodiscard]] double extractHarmonics(const std::vector<double>& cqBins, int baseFreqBin) const;
};
