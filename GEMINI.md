# Pitchenga

Pitchenga is a real-time music visualization audio plugin and standalone application built with C++20 and the JUCE
framework. It is inspired by the Java-based "HarmonEye" project.

## Project Overview

The project aims to provide two main visualization tools:

1. **Chromagram Circle:** A colorful, circular Chromagram chart that visualizes music as pitch classes binned across all
   octaves, using FFT (Fast Fourier Transform).
2. **Pitch Tracker:** A monophonic pitch detection tool (tuning triangle and linear octave slider) using MPM (McLeod
   Pitch Method).

### Key Technologies

- **Language:** C++20
- **Framework:** [JUCE](https://juce.com/) (using `juce_audio_utils`, `juce_dsp`, `juce_gui_basics`, etc.)
- **Build System:** CMake
- **IDE:** CLion
- **Mathematical Foundations:** FFT for polyphonic visualization and MPM for monophonic tracking.

## Directory Structure

- `Source/`: Core plugin source files (currently pending implementation).
    - `PluginProcessor.cpp/h`: Audio processing and spectral analysis logic.
    - `PluginEditor.cpp/h`: UI and visualization rendering code.
- `JUCE/`: Local copy/submodule of the JUCE framework.
- `doc/`: Documentation and planning.
    - `init.md`: **CRITICAL READING.** Contains the "Hybrid Strategy" for porting from Java, architectural decisions,
      and licensing considerations.
- `AGENTS.md`: Behavioral guidelines for AI agents to ensure high-quality, idiomatic C++ output and avoid common
  pitfalls.
- `main.cpp`: Default boilerplate file (not used by the primary plugin target).
- `cmake-build-debug/`: Build artifacts and intermediate files.

## Building and Running

The project is configured to build both **AU (Audio Unit)** and **Standalone** formats.

### Prerequisites

- CMake 3.22+
- A C++20 compatible compiler (Clang/Xcode on macOS).

### Build Commands

```bash
# Generate build files
cmake -B cmake-build-debug

# Build the project
cmake --build cmake-build-debug
```

### Running

- **Standalone:** The executable can be found in `cmake-build-debug/Pitchenga_artefacts/Debug/Standalone/Pitchenga.app`.
- **AU Plugin:** The build system is configured to automatically copy the AU component to
  `~/Library/Audio/Plug-Ins/Components/` upon successful compilation (`COPY_PLUGIN_AFTER_BUILD TRUE`).

## Development Conventions

### "Hybrid Strategy" (from `doc/init.md`)

1. **Idiomatic JUCE Shell:** Always build the outer shell (UI, audio routing) using idiomatic JUCE/C++ patterns. Do not
   attempt to direct-port Java architecture.
2. **Math Snippets:** Translate core mathematical logic (e.g., color mapping, bin folding) snippet-by-snippet from the
   original Java source to optimized C++.
3. **Graphics:** Use `juce::Graphics` for 2D rendering. Avoid OpenGL unless performance benchmarks specifically require
   it.

### AI Interaction Guidelines

- Refer to `AGENTS.md` for specific rules on code generation and interaction.
- Prioritize C++20 features and memory-safe JUCE patterns.
- Ensure all external libraries use permissive licenses (MIT, BSD, or Apache 2.0). **Avoid GPL/copyleft code.**

## Roadmap / TODO

- [ ] Implement `PluginProcessor` and `PluginEditor` boilerplate.
- [ ] Port "Rainbow Math" from Java for the Chromagram circle.
- [ ] Integrate an MIT-licensed MPM library (e.g., `adamski/pitch_detector`) for monophonic tracking.
- [ ] Add unit tests for DSP logic.
