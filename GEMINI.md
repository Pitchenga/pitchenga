# Pitchenga

Pitchenga is a real-time music visualization audio plugin and standalone application built with C++20 and the JUCE
framework.

## Directory Structure

- `Source/`: Core plugin source files (currently pending implementation).
    - `PluginProcessor.cpp/h`: Audio processing and spectral analysis logic.
    - `PluginEditor.cpp/h`: UI and visualization rendering code.
- `JUCE/`: Local copy/submodule of the JUCE framework.
- `doc/`: Documentation and planning.
- `cmake-build-debug/`: Build artifacts and intermediate files.

## Building and Running

The project is configured to build both **AU (Audio Unit)** and **Standalone** formats.

### Build Commands

```bash
# Generate build files
cmake -B cmake-build-debug

# Build the project
cmake --build cmake-build-debug

# Run the application
killall Pitchenga; ./cmake-build-debug/Pitchenga_artefacts/Debug/Standalone/Pitchenga.app/Contents/MacOS/Pitchenga
```

### Running

- **Standalone:** The executable can be found in `cmake-build-debug/Pitchenga_artefacts/Debug/Standalone/Pitchenga.app`.
- **AU Plugin:** The build system is configured to automatically copy the AU component to
  `~/Library/Audio/Plug-Ins/Components/` upon successful compilation (`COPY_PLUGIN_AFTER_BUILD TRUE`).

## Development Conventions

- **Idiomatic JUCE Shell:** Always build the outer shell (UI, audio routing) using idiomatic JUCE/C++ patterns. Do not
   attempt to direct-port Java architecture.
- **Graphics:** Use `juce::Graphics` for 2D rendering. Avoid using OpenGL.
- Prioritize C++20 features and memory-safe JUCE patterns.
- Ensure all external libraries use permissive licenses (MIT, BSD, or Apache 2.0). Avoid GPL/copyleft dependencies.

## Guidelines

- CRITICAL: Do NOT blindly remove existing comments or commented-out code. Before making any changes or using write_file, explicitly list all existing comments in the code you are about to change. After the change double-check and ensure that all pre-existing comments still exist. If any pre-existing comments became outdated then explicitly notify the user that an outdated comment is being removed.
- CRITICAL: Do NOT revert user's changes. Before making any changes - re-read all the files you are about to change. For any discrepancies between your context state and the actual state of a file, assume that the changes were made deliberately by the user and do NOT blindly revert them - recalibrate your planned changes accordingly. This is a hard rule and cannot be violated.
- CRITICAL: Always use descriptive names, except for "i", "x", "y".
- CRITICAL: Do not use numbers for steps in comments, as they quickly become outdated and hard to manage.
- Do not add "NEW" or to comments.
- Prefer named constants in header file rather than hard-coded values.
- Prefer separate functions to long code blocks with a comment.
- Use strict camel-case for acronyms, e.g. "SqlRdbmsDao" rather than "SQLRDBMSDAO".