#include "CqtEngine.h"
#include <cmath>
#include <iostream>
#include <algorithm>

CqtEngine::CqtEngine() {
    updateContext();
}

CqtEngine::CqtEngine(const Config& c) : config(c) {
    updateContext();
}

void CqtEngine::updateConfig(const Config& newConfig) {
    config = newConfig;
    updateContext();
}

int CqtEngine::nextPowerOf2(int value) const {
    value--;
    value |= (value >> 1);
    value |= (value >> 2);
    value |= (value >> 4);
    value |= (value >> 8);
    value |= (value >> 16);
    value++;
    return value;
}

double CqtEngine::centerFreq(int binIndex) const {
    return (baseFreq * std::pow(2.0, static_cast<double>(binIndex) * binsPerOctaveInv));
}

int CqtEngine::bandWidth(int binIndex) const {
    return static_cast<int>(std::ceil(q * config.samplingFreq / centerFreq(binIndex)));
}

static double calculateHammingIntegral() {
    // The Trapezoid integral of a normalized Hamming window over [0,1] is approximately 0.54
    return 0.54;
}

void CqtEngine::updateContext() {
    baseFreq = std::pow(2.0, -config.octaves) * config.maxFreq;
    binsPerOctave = config.semitonesPerOctave * config.binsPerSemitone;
    binsPerOctaveInv = 1.0 / binsPerOctave;
    totalBins = binsPerOctave * config.octaves;
    kernelBins = binsPerOctave * config.kernelOctaves;
    firstKernelBin = totalBins - kernelBins;
    q = 1.0 / (std::pow(2.0, binsPerOctaveInv) - 1.0);
    windowIntegral = calculateHammingIntegral();

    signalBlockSize = nextPowerOf2(bandWidth(firstKernelBin));
    normalizationFactor = 2.0 / (static_cast<double>(signalBlockSize) * windowIntegral);

    int fftOrder = static_cast<int>(std::round(std::log2(signalBlockSize)));
    fft = std::make_unique<juce::dsp::FFT>(fftOrder);
    
    fftWorkspace.resize(static_cast<size_t>(signalBlockSize), {0.0f, 0.0f});
}

std::vector<std::complex<float>> CqtEngine::temporalKernel(int kernelBinIndex) const {
    int size = bandWidth(kernelBinIndex + firstKernelBin);
    std::vector<std::complex<float>> coeffs(static_cast<size_t>(size), {0.0f, 0.0f});
    
    double sizeInv = 1.0 / size;
    double factor = 2.0 * juce::MathConstants<double>::pi * q * sizeInv;
    
    for (int i = 0; i < size; ++i) {
        // Standard Hamming Window
        double x = static_cast<double>(i) * sizeInv;
        double w = 0.54 - 0.46 * std::cos(2.0 * juce::MathConstants<double>::pi * x);
        double r = w * sizeInv;
        
        double theta = static_cast<double>(i) * factor;
        coeffs[static_cast<size_t>(i)] = { static_cast<float>(r * std::cos(theta)), static_cast<float>(r * std::sin(theta)) };
    }
    
    return coeffs;
}

std::vector<std::complex<float>> CqtEngine::conjugatedNormalizedSpectralKernel(int k) {
    auto tk = temporalKernel(k);
    
    // Left pad with zeros to match signalBlockSize
    std::vector<std::complex<float>> padded(static_cast<size_t>(signalBlockSize), {0.0f, 0.0f});
    int dataSize = std::min(static_cast<int>(tk.size()), signalBlockSize);
    int paddingSize = signalBlockSize - dataSize;
    for (int i = 0; i < dataSize; ++i) {
        padded[static_cast<size_t>(paddingSize + i)] = tk[static_cast<size_t>(i)];
    }
    
    std::vector<std::complex<float>> spectrum(static_cast<size_t>(signalBlockSize), {0.0f, 0.0f});
    
    // Complex Forward FFT
    fft->perform(padded.data(), spectrum.data(), false);
    
    // Normalize, Conjugate and Chop using threshold
    std::vector<std::complex<float>> result(static_cast<size_t>(signalBlockSize), {0.0f, 0.0f});
    for (int i = 0; i < signalBlockSize; ++i) {
        float re = spectrum[static_cast<size_t>(i)].real();
        float im = spectrum[static_cast<size_t>(i)].imag();
        
        float absVal = std::sqrt(re * re + im * im);
        
        // CHOP BEFORE NORMALIZATION (matching Java logic)
        if (absVal >= config.chopThreshold) {
            result[static_cast<size_t>(i)] = {
                static_cast<float>(static_cast<double>(re) * normalizationFactor),
                static_cast<float>(-static_cast<double>(im) * normalizationFactor) // Conjugation
            };
        }
    }
    
    return result;
}

void CqtEngine::computeSpectralKernels() {
    int rows = kernelBins;
    int cols = signalBlockSize;
    
    spectralKernels.resize(rows, cols);
    std::vector<Eigen::Triplet<std::complex<float>>> triplets;
    
    for (int k = 0; k < rows; ++k) {
        auto kernel = conjugatedNormalizedSpectralKernel(k);
        for (int i = 0; i < cols; ++i) {
            if (kernel[static_cast<size_t>(i)].real() != 0.0f || kernel[static_cast<size_t>(i)].imag() != 0.0f) {
                triplets.push_back(Eigen::Triplet<std::complex<float>>(k, i, kernel[static_cast<size_t>(i)]));
            }
        }
    }
    
    spectralKernels.setFromTriplets(triplets.begin(), triplets.end());
    spectralKernels.makeCompressed();
}

void CqtEngine::init() {
    computeSpectralKernels();
    cqtRes.resize(kernelBins);
    cqtRes.setZero();
}

void CqtEngine::transform(const std::vector<float>& timeDomainSignal, std::vector<std::complex<float>>& cqtSpectrumOut) {
    if (timeDomainSignal.size() < static_cast<size_t>(signalBlockSize)) return;
    
    // Perform complex Forward FFT on input signal
    for (int i = 0; i < signalBlockSize; ++i) {
        fftWorkspace[static_cast<size_t>(i)] = {timeDomainSignal[static_cast<size_t>(i)], 0.0f};
    }
    
    fft->perform(reinterpret_cast<const std::complex<float>*>(fftWorkspace.data()), 
                 reinterpret_cast<std::complex<float>*>(fftWorkspace.data()), false);
    
    // Wrap the spectrum via Eigen Map
    Eigen::Map<const Eigen::VectorXcf> signalFft(fftWorkspace.data(), signalBlockSize);
        
    // Standard Complex Sparse Matrix * Vector Multiplication into pre-allocated result
    cqtRes.noalias() = spectralKernels * signalFft;
    
    cqtSpectrumOut.resize(static_cast<size_t>(kernelBins));
    for (int i = 0; i < kernelBins; ++i) {
        cqtSpectrumOut[static_cast<size_t>(i)] = cqtRes(i);
    }
}
