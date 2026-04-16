# Pitchenga vs. HarmonEye: Implementation Comparison Report

This document compares the legacy Java implementation (HarmonEye) with the modern C++ JUCE-based port (Pitchenga).

## 1. Shared Core Logic (The Same)

* **CQT Algorithm:** Both implementations use a Constant-Q Transform with frequency-domain kernels. The kernels are
  precomputed, thresholded for sparsity, and applied via sparse matrix multiplication.
* **Multi-Rate Processing:** Both use a decimation-by-2 cascade to analyze multiple octaves with high frequency
  resolution without increasing the FFT size exponentially.
* **Analysis Pipeline:**
    1. **Harmonic Pattern Detection:** Weights bins by checking for the presence of harmonic overtones.
    2. **Spectral Equalizer:** Shaves off noise floor using a rolling median filter.
    3. **Octave Folding:** Aggregates spectral data across octaves into a single 12-semitone circle.
    4. **Exponential Smoothing:** Uses asymmetric weights for fast attack and slow decay to create "punchy" visuals.
* **Visual Mapping:** Both map pitch classes to specific angles (C = 0°) and use a 12-tone color wheel (
  Newtonian/Solfege-inspired).

## 2. Architectural Differences

| Feature            | Legacy Java (HarmonEye)                | Modern C++ (Pitchenga)                           |
|:-------------------|:---------------------------------------|:-------------------------------------------------|
| **Framework**      | Swing + JOGL (OpenGL)                  | JUCE (C++20)                                     |
| **Audio I/O**      | Java Sound API                         | JUCE `AudioProcessor` (ASIO/CoreAudio)           |
| **Linear Algebra** | Custom Sparse Matrix class             | **Eigen Library** (`Eigen::SparseMatrix`)        |
| **FFT**            | JTransforms (`DoubleFFT_1D`)           | `juce::dsp::FFT`                                 |
| **Threading**      | `TimerTask` polling                    | **Dedicated Real-time Thread** + Lock-free FIFOs |
| **Decimation**     | Buffer-bank with 6th order Butterworth | Cascade decimation using `juce::dsp::IIR`        |
| **Rendering**      | Immediate Mode OpenGL                  | `juce::Graphics` (Path-based)                    |
| **Normalization**  | Mean-based and Max-based               | Refined Max-based with safety clamps             |

## 3. Notable Porting Decisions

### Eigen vs. Custom Java Math

The Java version implemented its own sparse matrix multiplication. Pitchenga replaces this with the industry-standard *
*Eigen** library. This provides significant performance gains and allows for more aggressive `chopThreshold` settings,
leading to cleaner visualization with less CPU overhead.

### Real-time Safety

HarmonEye's audio processing and analysis were closely tied to the UI timer. Pitchenga decouples these entirely:

1. **Audio Thread:** Only handles decimation and pushing to FIFOs.
2. **Worker Thread:** Handles the heavy CQT and analysis math.
3. **UI Thread:** Only handles rendering the latest results.
   This "JUCE-idiomatic" approach prevents audio dropouts and ensures a smooth 30-60 FPS UI even under heavy DSP load.

### Smoothing Logic

The Java `ExpSmoother` had a check `if (currentFrame[i] < 0.5 || data[i] > currentFrame[i])`. The C++ version simplifies
this to a more standard `if (currentFrame[i] > data[i])` for instant attack, providing a more responsive visual feel for
percussive notes.

## 4. Conclusion

Pitchenga is a "math-accurate" port that preserves the sophisticated pitch-tracking logic of HarmonEye while modernizing
the infrastructure for use as a professional Audio Plugin (VST3/AU).
