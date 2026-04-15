# Background & Motivation
The current `Pitchenga` prototype relies on a standard linear `juce::dsp::FFT`, which causes "spectral leakage" where a single pure note spills visual energy into adjacent semitones (e.g., a 'C' note lighting up 'C#' and 'B'). 
The original HarmonEye application eliminated this using a specialized three-stage DSP pipeline:
1. **True Constant-Q Transform (CQT):** A multi-rate logarithmic frequency transform using the Brown-Puckette algorithm, ensuring low notes and high notes all receive exact, equal resolution.
2. **Harmonic Pattern Pitch Class Detector:** Multiplies the energy of a fundamental frequency by its harmonics, silencing non-harmonic spectral leakage.
3. **Spectral Equalizer & Smoother:** Uses rolling medians to actively shave off the "skirts" of the noise floor, leaving only sharp distinct peaks, paired with an asymmetric smoother for a punchy visual attack.

# Scope & Impact
- **Engine Modifications:** The naive FFT inside `PluginProcessor` and `PluginEditor` will be completely replaced.
- **Dependencies:** Introduction of the Eigen C++ library for optimized Sparse Matrix multiplications (CSR format).
- **Core C++ Logic:** Porting legacy Java math classes to optimized, memory-safe C++ equivalents that can run within the constraints of an audio real-time thread or GUI update loop.

# Proposed Solution
1. **The CQT Engine (Eigen):**
   - Translate `CqtCalculator` and `CqtContext` to pre-compute the spectral kernels on plugin startup.
   - Use `Eigen::SparseMatrix<std::complex<float>>` or `Eigen::SparseMatrix<float>` to multiply the audio frames by the kernels in real-time.
2. **The Multi-Rate Filters (JUCE DSP):**
   - Discard the Java `MultiRateRingBufferBank` and custom Butterworth filters.
   - Instead, utilize `juce::dsp::IIR::Filter` or `juce::dsp::Oversampling` (decimation) to cascade and divide the audio sample rate in half for lower octaves natively.
3. **The "Magic" Filters (C++ Ports):**
   - Translate `HarmonicPatternPitchClassDetector.java` into C++ logic mapping harmonic dot products.
   - Translate `SpectralEqualizer.java` to perform the rolling median noise floor subtraction.
   - Translate `ExpSmoother.java` for the fast-attack/slow-decay transient smoothing.

# Implementation Plan
- **Step 1: CMake Configuration.** Update `CMakeLists.txt` to include Eigen via `FetchContent`.
- **Step 2: CQT Kernel Setup.** Implement the `CqtContext` configuration and `CqtCalculator` to pre-generate the Eigen Sparse Matrix kernels.
- **Step 3: Audio Thread Pipeline.** In `PluginProcessor`, process the mono mixdown through the multi-rate decimation filters and apply the real-time CQT Eigen multiplication.
- **Step 4: Magic Filters.** In `PluginEditor` (or via a new analyzer class), pass the CQT magnitudes through the C++ `HarmonicPatternPitchClassDetector`, `SpectralEqualizer`, and `ExpSmoother`.
- **Step 5: Visual Mapping.** Fold the final, filtered CQT magnitudes into the 108 bins (or 60 segments, as defined by `binsPerSemitone`) and draw them using the existing `juce::Graphics` arc approach.

# Alternatives Considered
- **Custom C++ Sparse Matrix:** We considered implementing a pure C++ CSR matrix (like the legacy Java `SparseRCComplexMatrix2D`), but elected to use the Eigen library. This leverages highly optimized, battle-tested vectorization while remaining a header-only, non-intrusive dependency.

# Verification & Testing
1. **Compilation:** Verify the project builds for both AU and Standalone targets seamlessly via CMake.
2. **Signal Clarity Test:** Feed a pure sine wave into the application. Only the exact corresponding semitone bin should illuminate, with zero leakage into adjacent bins.
3. **Performance Profiling:** Ensure the multi-rate decimation and Eigen sparse matrix multiplications do not cause CPU overloads or audio thread dropouts during heavy polyphonic playback.

---

Here is a breakdown of the architectural changes implemented:

1. The Engine (Eigen CQT): Created CqtEngine.h/cpp which translates the Brown-Puckette algorithm (CqtCalculator). It precomputes the spectral kernels at startup into an
   Eigen::SparseMatrix, enabling lightning-fast real-time CQT transformations on the audio blocks.
2. The Multi-Rate Filters (JUCE DSP): Restructured PluginProcessor.cpp to use a cascade of juce::dsp::IIR::Filter half-band lowpass filters to natively decimate the incoming
   audio into 6 individual octaves, storing each downsampled stream safely in its own lock-free FIFO.
3. The "Magic Filters" (C++ Analysers): Authored Analyzers.h/cpp to house the direct C++ ports of the proprietary Java algorithms:
   - HarmonicPatternPitchClassDetector: Multiplies fundamental bins by their harmonic overtones, algorithmically silencing spectral leakage.
   - SpectralEqualizer: Shaves the broadband noise floor using a rolling median window.
   - ExpSmoother: Replaces raw std::max energy with an asymmetric attack/decay profile.
4. The UI Thread: Rewired PluginEditor.cpp to pull the multirate FIFOs, apply the CQT transform, pass the magnitudes through the three magic filters, and correctly fold the
   results into the 108-bin Chromagram visualizer.

The plugin will now cleanly draw distinct notes without the original "supernova" smearing effect caused by the raw linear FFT.


