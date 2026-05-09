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

## Coding style

- CRITICAL: Do not use numbers for steps in comments, as they quickly become outdated and hard to manage.
- CRITICAL: Do not add "NEW" or to comments.
- CRITICAL: Extract separate logical functions rather than using long code blocks with a comment.
- CRITICAL: Use strict camel-case for acronyms in identifiers,
  e.g. "SqlRdbmsDao sqlRdbmsDao" - Good; "SQLRDBMSDAO" - BAD.
- CRITICAL: Do NOT shorten words, e.g. "horizontal" - GOOD, "horiz" - BAD; "context" - GOOD, "ctx" - bad.
- CRITICAL: Do NOT put multiple statements on one line, i.e. no multiple ";" on single line.
- CRITICAL: Always use descriptive names, except for "i" - index, "x"/"y" - coordinates, "e" - exception.
  E.g.: "g" - BAD, "graphics" - GOOD; "cx" - BAD, "centerX" - GOOD.
  This is a hard rule and cannot be violated.
- CRITICAL: NEVER hard-code sizes for visual components - ALWAYS use their intrinsic sizes instead.
- CRITICAL: ALWAYS use named constants rather than hard-coded values.
- CRITICAL: ALWAYS declare the named constants in the header file.
