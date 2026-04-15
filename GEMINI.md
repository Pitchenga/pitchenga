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

- **Idiomatic JUCE Shell:** Always build the outer shell (UI, audio routing) using idiomatic JUCE/C++ patterns. Do not
   attempt to direct-port Java architecture.
- **Math Snippets:** Translate core mathematical logic (e.g., color mapping, bin folding) snippet-by-snippet from the
   original Java source to optimized C++.
- **Graphics:** Use `juce::Graphics` for 2D rendering. Avoid using OpenGL.
- Prioritize C++20 features and memory-safe JUCE patterns.
- Ensure all external libraries use permissive licenses (MIT, BSD, or Apache 2.0). Avoid GPL/copyleft dependencies.


## Coding style

- Avoid generating duplicated code: refactor and reuse instead.
- CRITICAL: DO NOT remove existing comments or commented-out code. This is a hard rule and cannot be violated.
- CRITICAL: Before changing any file (write_file), explicitly list all exising comments in the code you are about to change. After the change double-check and ensure that all pre-existing comments still exist and no new comments were added. If any pre-existing comments became outdated by the change explicitly state it to the user, but do not remove them.

# Guidelines

Behavioral guidelines to reduce common LLM coding mistakes. Merge with project-specific instructions as needed.

**Tradeoff:** These guidelines bias toward caution over speed. For trivial tasks, use judgment.

## 1. Think Before Coding

**Don't assume. Don't hide confusion. Surface tradeoffs.**

Before implementing:
- State your assumptions explicitly. If uncertain, ask.
- If multiple interpretations exist, present them - don't pick silently.
- If a simpler approach exists, say so. Push back when warranted.
- If something is unclear, stop. Name what's confusing. Ask.

## 2. Simplicity First

**Minimum code that solves the problem. Nothing speculative.**

- No features beyond what was asked.
- No abstractions for single-use code.
- No "flexibility" or "configurability" that wasn't requested.
- No error handling for impossible scenarios.
- If you write 200 lines and it could be 50, rewrite it.

Ask yourself: "Would a senior engineer say this is overcomplicated?" If yes, simplify.

## 3. Surgical Changes

**Touch only what you must. Clean up only your own mess.**

When editing existing code:
- Don't "improve" adjacent code, comments, or formatting.
- Don't refactor things that aren't broken.
- Match existing style, even if you'd do it differently.
- If you notice unrelated dead code, mention it - don't delete it.

When your changes create orphans:
- Remove imports/variables/functions that YOUR changes made unused.
- Don't remove pre-existing dead code unless asked.

The test: Every changed line should trace directly to the user's request.

## 4. Goal-Driven Execution

**Define success criteria. Loop until verified.**

Transform tasks into verifiable goals:
- "Add validation" → "Write tests for invalid inputs, then make them pass"
- "Fix the bug" → "Write a test that reproduces it, then make it pass"
- "Refactor X" → "Ensure tests pass before and after"

For multi-step tasks, state a brief plan:
```
1. [Step] → verify: [check]
2. [Step] → verify: [check]
3. [Step] → verify: [check]
```

Strong success criteria let you loop independently. Weak criteria ("make it work") require constant clarification.

---

**These guidelines are working if:** fewer unnecessary changes in diffs, fewer rewrites due to overcomplication, and clarifying questions come before implementation rather than after mistakes.
