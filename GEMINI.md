# Pitchenga

Pitchenga is a real-time music visualization application and audio plugin.

## Stack

- Language and version: C++20 - use modern syntax with Class Template Argument Deduction -
  no redundant type arguments and qualifiers.
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

- `Util::log("Failed loading plugin, name=" + name + ", error=" + error + ",")`

## Coding style

- CRITICAL: Do not use numbers for steps in comments, as they quickly become outdated and hard to manage.
- CRITICAL: Do not add "NEW" or to comments.
- CRITICAL: Extract separate logical functions rather than using long code blocks with a comment.
- CRITICAL: Use strict camel-case for acronyms in identifiers,
  e.g. "SqlRdbmsDao sqlRdbmsDao" - Good; "SQLRDBMSDAO" - BAD.
- CRITICAL: Do NOT shorten words (except for abbreviations and common ones like "app"),
  e.g. "horizontal" - GOOD, "horiz" - BAD; "context" - GOOD, "ctx" - bad.
- CRITICAL: Always use descriptive names, except for: "i" - index, "x"/"y" - coordinates, "e" - exception,
  "a"/"b"/etc for e.g. sorting parameters.
  E.g.: "g" - BAD, "graphics" - GOOD; "cx" - BAD, "centerX" - GOOD; "sl" - BAD, "scopedLock" - GOOD.
  This is a hard rule and cannot be violated.
- CRITICAL: Do NOT put multiple statements on one line, i.e. no multiple ";" on single line.
- CRITICAL: NEVER hard-code sizes for visual components - ALWAYS use their intrinsic sizes instead.
- CRITICAL: ALWAYS use named constants rather than hard-coded values.
- CRITICAL: ALWAYS declare the named constants in the header file.
- CRITICAL: Do NOT expose private identifiers in files submitted to version control (including documentation):
  this is a public repo. For private identifiers, key, and passwords use GitHub secrets and local .env file.
