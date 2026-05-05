# Pitchenga

Pitchenga is a real-time music visualization application and audio plugin.

## Stack

- Language and version: C++20
- Framework(s): JUCE
- Package manager: CMake
- Runtime / deployment target: Debug

## Commands

- Install:
  `cmake -G Ninja -B cmake-build-debug`
- Build - run after completing every request and check if it succeeds:
  `cmake --build cmake-build-debug`
- Run locally:
  `killall Pitchenga; ./cmake-build-debug/Pitchenga_artefacts/Debug/Standalone/Pitchenga.app/Contents/MacOS/Pitchenga`
- Release build: `cmake -G Ninja -B cmake-build-release && cmake --build cmake-build-release`

## Layout

- `Source/`: Core plugin source files.
- `doc/`: Documentation and planning.
- `lib/`: Third-party libraries
- `cmake-build-debug/`: Build artifacts and intermediate files.

## Logging

- `Util::debug("Failed loading plugin, name=" + name + ", error=" + error + ",")`

