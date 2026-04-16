# Investigation Report: Velocity Discrepancy in High-Pitch Notes

## Executive Summary

The C++ implementation of Pitchenga exhibits a significant "velocity drop" for high-pitch notes (C6-C8) compared to the
legacy Java version. This investigation identified four primary causes: an explicit gain tilt favoring low frequencies,
a harmonic detection algorithm that loses terms at high frequencies, a non-linear rank-based amplification stage, and
differences in the decibel rescaling range.

---

## 1. Primary Cause: Explicit Octave Gain Tilt

In `PluginEditor.cpp` (CqtWorkerThread), an explicit gain adjustment is applied to each octave:

```cpp
// C++ Source/PluginEditor.cpp
float octaveTilt = 1.0f + (static_cast<float>(oct) * 0.15f);
```

* **Logic Error:** `oct = 0` represents the highest frequency octave (original sample rate). This octave receives a
  multiplier of **1.0x**.
* **Low Frequency Bias:** `oct = 5` represents the lowest frequency octave. It receives a multiplier of **1.75x**.
* **Legacy Comparison:** The Java implementation in `MusicAnalyzer.java` applies **zero tilt**; all octaves are
  processed with equal weight.
* **Impact:** High-pitch notes are effectively attenuated by ~5dB relative to low-pitch notes before any analysis even
  begins.

## 2. Harmonic Detector Edge Effects

Both implementations use a fixed-size spectrum for analysis (approx. 6 octaves). However, the way harmonics are summed
creates a natural bias:

* **Low Notes:** A note at C2 (near the start of the spectrum) can "find" all 10 requested harmonics within the 648-bin
  range.
* **High Notes:** A note at C7 (near the end of the spectrum) has its harmonics (2nd, 3rd, etc.) fall **outside** the
  analyzed bin range.
* **Missing Terms:** In `Analyzers.cpp`, the `extractHarmonics` function simply skips these off-spectrum harmonics. This
  results in a significantly lower `dotProduct` for high notes.
* **Missing Normalization:** The legacy Java code uses `normalizeViaMean` in `HarmonicPatternPitchClassDetector.java` to
  scale the harmonic spectrum back to the original energy level. While the C++ code attempt a similar normalization, the
  combination with the `octaveTilt` results in the high-frequency "thinning out."

## 3. Rank-Based Amplification Penalty

The C++ version introduced a "Rank-Based Amplification" stage in `PluginEditor.cpp` that does not exist in the Java
version:

```cpp
// C++ Source/PluginEditor.cpp
double multiplier = 0.4 + (static_cast<double>(i) / static_cast<double>(binsPerOctave)) * 0.8;
velocity *= multiplier;
```

This logic amplifies the loudest bins and attenuates the quieter ones. Because high-pitch notes are already weakened by
the `octaveTilt` and the harmonic detector bias, they consistently fall into a lower "rank," receiving a smaller
multiplier (closer to 0.4x) compared to the low-pitch notes (which stay closer to 1.2x). This creates a "rich get
richer" effect for low frequencies.

## 4. Decibel Rescaling Range

The two versions map the logarithmic spectrum to the [0.0, 1.0] visual range using different floors:

* **Java:** Uses a 16-bit floor of **-96.3 dB**. The scaling is `1 - (db / -96.3)`.
* **C++:** Uses a fixed floor of **-80.0 dB**. The scaling is `(db + 80) / 80`.

The C++ scaling is "steeper." A signal at -40dB results in 0.58 in Java but only 0.50 in C++. High-frequency components,
which are naturally quieter in most audio signals, hit the -80dB floor and disappear much faster in the C++ version.

---

## Recommended Corrective Actions

1. **Remove the Octave Tilt:** Set `octaveTilt = 1.0f` for all octaves to match the legacy behavior.
2. **Harmonic Normalization:** Implement a more robust normalization in `HarmonicPatternPitchClassDetector` that
   accounts for the number of harmonics actually found (e.g., divide the dot product by the number of included terms).
3. **Adjust dB Floor:** Lower the C++ noise floor to -96dB to match the Java dynamic range.
4. **Bypass Rank-Amplification:** Disable or significantly soften the rank-based scaling to prevent it from suppressing
   secondary high-frequency notes.
