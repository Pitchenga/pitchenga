#pragma once

#include <vector>
#include <complex>
#include <memory>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <juce_dsp/juce_dsp.h>

class Cqt {
public:
    struct Config {
        int octaves = 6;
        int kernelOctaves = 1;
        double maxFreq = 4186.0090448064; // C8
        double samplingFreq = 44100.0;
        int semitonesPerOctave = 12;
        int binsPerSemitone = 9;
        double chopThreshold = 0.005;
    };

    Cqt();
    explicit Cqt(const Config& config);
    ~Cqt() = default;

    void updateConfig(const Config& newConfig);
    void init();

    const Config& getConfig() const { return config; }

    // Transform one block of time-domain signal.
    // The signal MUST be exactly getSignalBlockSize() in length.
    void transform(const std::vector<float>& timeDomainSignal, std::vector<std::complex<float>>& cqtSpectrumOut);

    int getSignalBlockSize() const { return signalBlockSize; }
    int getTotalBins() const { return totalBins; }
    int getBinsPerOctave() const { return binsPerOctave; }
    int getOctaves() const { return config.octaves; }
    int getKernelBins() const { return kernelBins; }

private:
    Config config;
    double baseFreq;
    int binsPerOctave;
    double binsPerOctaveInv;
    int totalBins;
    int kernelBins;
    int firstKernelBin;
    double q;
    double windowIntegral;
    int signalBlockSize;
    double normalizationFactor;

    Eigen::SparseMatrix<std::complex<float>, Eigen::RowMajor> spectralKernels;
    Eigen::VectorXcf cqtRes;
    std::unique_ptr<juce::dsp::FFT> fft;

    // Buffers for FFT
    std::vector<std::complex<float>> fftWorkspace;

    void updateContext();
    void computeSpectralKernels();
    double centerFreq(int binIndex) const;
    int bandWidth(int binIndex) const;
    std::vector<std::complex<float>> temporalKernel(int kernelBinIndex) const;
    std::vector<std::complex<float>> conjugatedNormalizedSpectralKernel(int k);
    int nextPowerOf2(int value) const;
};
