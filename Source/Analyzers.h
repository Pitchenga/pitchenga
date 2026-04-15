#pragma once

#include <vector>

// 1. Exponential Smoother
// Asymmetric smoothing for punchy visual attack and slower decay.
class ExpSmoother {
public:
    ExpSmoother(int size, double currentWeight);
    std::vector<double> smooth(const std::vector<double>& currentFrame);
    
private:
    std::vector<double> data;
    double currentWeight;
    double previousWeight;
};

// 2. Spectral Equalizer
// Uses a rolling median filter to shave off the noise floor and the "skirts" of spectral leakage.
class SpectralEqualizer {
public:
    SpectralEqualizer(int size, int windowSize);
    std::vector<double> filter(const std::vector<double>& values);
    
private:
    int size;
    int windowSize;
    std::vector<double> filteredValues;
    std::vector<double> window;
    std::vector<double> tempWindow;
};

// 3. Harmonic Pattern Pitch Class Detector
// Weights each bin by checking if its harmonic overtones exist, silencing non-harmonic noise.
class HarmonicPatternPitchClassDetector {
public:
    HarmonicPatternPitchClassDetector(int binsPerOctave, int binsPerHalftone, int harmonicCount = 10);
    std::vector<double> detectPitchClasses(const std::vector<double>& cqBins);
    
private:
    int harmonicCount;
    int binsPerOctave;
    int binsPerHalftoneHalf;
    double harmonicCountMinusOneInv;
    
    std::vector<double> harmonicBins;
    std::vector<int> harmonicBinsIndexes;
    
    double extractHarmonics(const std::vector<double>& cqBins, int baseFreqBin) const;
};
