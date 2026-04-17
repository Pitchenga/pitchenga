You are officially the Lead Architect, and it is time to start assigning the heavy lifting to your AI developers.

Here is the exact strategic roadmap to get from a blank directory to replacing the "Hissy Snake Nest," followed by the
master prompt you will feed to the Gemini CLI to execute Phase 1.

### The Pitchenga Master Development Plan

**Phase 1: The Core Chromagram Engine (Current Priority)**

* **Objective:** Get the basic AU/Standalone shell compiling, intercept the audio buffer, run the hardware-accelerated
  FFT, and draw the 12-bin rainbow visualizer.
* **Result:** A working Logic plugin that perfectly replaces your quadruple-output/Java hack for visual tracking, giving
  you a zero-latency, zero-hiss visualizer.

**Phase 2: The Precision Pitch Tracker**

* **Objective:** Drop in the MIT-licensed `adamski/pitch_detector` (MPM algorithm). Run it in parallel with the FFT on
  the audio thread.
* **Result:** Add the "Tuning Triangle" and the linear octave slider overlay to the UI so you can perfectly lock in your
  vocal pitch over the chords.

**Phase 3: The Wurlitzer Mini-DAW**

* **Objective:** Implement `AudioProcessorGraph` and `AudioPluginFormatManager` inside the standalone build.
* **Result:** The standalone app can now host the Splice Wurlitzer AU natively, grab your MIDI keyboard directly, and
  feed the internal audio straight to the visualizer.

**Phase 4: The Final Boss (ScreenCaptureKit)**

* **Objective:** Use macOS C++ preprocessor directives to capture the system desktop audio directly into the standalone
  app.
* **Result:** You completely delete BlackHole and LadioCast from your Mac. The software matrix is pristine, and the
  hardware puzzle is permanently solved.

---

### The Gemini CLI Master Prompt (Phase 1)

Copy and paste this exact prompt into your Gemini CLI. It sets the absolute strictest C++ DSP guardrails so the AI
doesn't hallucinate slow, unsafe Java concepts.

> **System Role:** You are an expert C++ Audio DSP Developer specializing in the JUCE Framework (v8) and modern C++20.
>
> **Context:** I am building a dual-format (AU and Standalone) audio plugin called "Pitchenga". The `CMakeLists.txt` is
> already completely configured. The `Source` directory is currently empty.
>
> **The Task:** Write the initial implementation for the four core files: `PluginProcessor.h`, `PluginProcessor.cpp`,
`PluginEditor.h`, and `PluginEditor.cpp`.
>
> **Architectural Requirements:**
> 1. **The Audio Thread (`processBlock`):** >    * Intercept the incoming audio buffer and mix it down to mono if it is
     stereo.
     >    * Push the mono audio samples into a lock-free FIFO queue. **Do not** allocate memory, use locks, or run heavy
     math on the audio thread.
> 2. **The DSP/Math:**
     >    * Use `juce::dsp::FFT` (size 2048 or 4096).
     >
* The GUI thread should pull from the lock-free FIFO, perform the FFT, and calculate a "Chromagram" (Pitch Class
  Profile).
>    * You must fold the entire frequency spectrum logarithmically into 12 distinct frequency bins representing the 12
       musical notes (C, C#, D, etc., ignoring octave).
> 3. **The GUI Thread (`PluginEditor`):**
     >    * Inherit from `juce::Timer` and tick at 30fps.
     >
* Use strictly `juce::Graphics` in the `paint()` method (no OpenGL).
>    * Draw a dynamic, resizable UI. For this initial phase, draw a circular layout consisting of 12 segments (like a
       clock face).
>    * Map the energy of the 12 Chromagram bins to the opacity or radius of these 12 segments. Assign a distinct color
       to each of the 12 musical notes.
>
> **Output:** Provide the raw C++ code for all four files. Prioritize thread safety (strict separation between audio and
> UI threads) and idiomatic JUCE C++20 structure. Do not explain the CMake setup; just provide the C++ implementations.
> 

