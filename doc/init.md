Pitchenga is a project aimed to build:

- a standalone desktop/mobile application that hosts AU/VST plugins.
- a VST/AU plugin that visualizes music in real-time as pitches binned in a colorful circular chromagram chart inspired
  by HarmonEye.

### The Danger of the "Full Dump"

If you point the AI to your entire Java HarmonEye repository and say, "Turn this into a C++ JUCE app," you will run into
two major walls:

1. **Context Confusion:** The AI will try to translate everything at once—the UI, the audio routing, the math—and will
   likely hallucinate structural connections that don't make sense in JUCE.
2. **Java-isms in C++:** Java and C++ handle memory and objects very differently. If the AI tries to directly translate
   Java architecture, it will write "Java-flavored C++," which is a notoriously slow, inefficient mess for real-time
   audio.

### The "Hybrid" Strategy

**1. Start the JUCE Architecture from Scratch**
Treat the foundation of the app like a brand-new build. Let JUCE be JUCE.

* **The Prompt:** Ask the AI to build the outer shell idiomatically. *"I am using CMake and JUCE in CLion. Write the
  boilerplate for an audio plugin that has a basic resizable UI window and intercepts the audio buffer."*
* **The Result:** The AI will give you perfectly modern, memory-safe C++ setup code that you couldn't easily port from
  Java anyway.

**2. Transplant your "Rainbow Math" (Snippet by Snippet)**
This is where your Java code becomes incredibly valuable. You already did the hard work of mapping frequencies to those
specific, custom-interpolated rainbow colors and intensity grades.

* **The Prompt:** Isolate a single mathematical function in your Java code (e.g., the color interpolation method) and
  paste just that snippet. *"Here is my Java method for mapping FFT bins to RGB values. Translate this strict
  mathematical logic into an optimized C++ function I can use in my JUCE app."*
* **The Result:** The AI will perfectly adapt your custom logic into fast C++ math. You get to keep your exact beautiful
  rainbow behavior without rewriting the logic.

**3. The UI Layer: Ditch OpenGL (For Now)**
Since agility is the name of the game for a hobby project, **do not touch OpenGL.** OpenGL in C++ is incredibly verbose
and requires hundreds of lines of boilerplate just to draw a single triangle.

* JUCE has an incredibly powerful, native 2D graphics engine called `juce::Graphics`. It is heavily optimized,
  hardware-accelerated, and operates a lot like HTML5 Canvas or Java's `Graphics2D`.
* **The Prompt:** *"Using `juce::Graphics`, write the `paint()` method to draw 12 vertical bars that dynamically change
  color and height based on an array of floats."*

By letting the AI build the C++ house from scratch, but bringing your custom Java "furniture" (the math and color logic)
inside, you will have a working prototype in a fraction of the time.

Since your Java code likely relied on TarsosDSP for the heavy lifting of the pitch detection, are you planning to ask
the AI to translate that specific math as well, or would you prefer to hook into JUCE's native, highly optimized C++ FFT
class?

---

I am highly confident in how to map this exact dual-architecture from Java to C++, as this specific combination of
techniques is a staple in professional audio analysis.

This is a brilliantly designed system. By separating the visualizations, you perfectly bridged the gap between harmonic
theory and practical performance. The chromatic circle (which mathematically folds all octaves together) is the ultimate
way to visually understand chords and key signatures, while the linear slider and tuning triangle give you that
surgical, monophonic precision.

Because your architecture relies on two fundamentally different ways of looking at audio, here is exactly how you
replace both halves in your new C++ JUCE environment.

### 1. The HarmonEye Circle (Your Custom Math)

Since the binned, octave-folding math is entirely your own custom code, the "snippet-by-snippet" AI translation we
discussed is going to work flawlessly.

You will likely just need to feed the AI the core mathematical loop where you calculate the energy of those specific
bins and map them to your custom rainbow array. In JUCE, you will feed your incoming audio buffer into the
`juce::DSP::FFT` class, and then run your translated custom math on the resulting frequency data to drive the height and
color intensity of the circle.

### 2. Replacing TarsosDSP (The Exact Pitch Tracker)

This is where you have to make a distinct engineering choice. FFT (Fast Fourier Transform) is notoriously bad at exact
pitch detection (like for a guitar tuner) because the frequencies are grouped into blocky "bins." TarsosDSP got around
this by using time-domain pitch tracking algorithms.

You have two incredibly clean paths to replace TarsosDSP in your C++ build:

* **The AI-Coding Path (Write Your Own):** The most famous and accurate algorithm for real-time monophonic pitch
  detection is called **YIN**. It is what TarsosDSP defaults to, and it is entirely mathematical. You can literally
  prompt your AI with: *"Write a lightweight C++ implementation of the YIN pitch detection algorithm to process an array
  of audio floats and return the fundamental frequency in Hz."* It is mathematically well-documented, and an LLM can
  easily generate a clean, standalone C++ function for it.
* **The Single-Header Library Path:** The C++ community is famous for "single-header libraries" (tiny, ultra-optimized
  files you just drop into your project with zero complicated setup). There are lightweight libraries like `Q` (an
  open-source C++ DSP library) that have highly optimized, real-time pitch trackers built specifically for musical
  tuning.

Either way, you will run the audio buffer through your custom FFT math for the rainbow circle, and simultaneously run
that exact same buffer through your YIN algorithm to drive the floating triangle and the linear octave slider.

Do you remember which specific pitch detection algorithm you were invoking inside TarsosDSP (like YIN, MPM, or YinBird),
so we know exactly what mathematical behavior to ask the AI to replicate?

---

You are absolutely spot on regarding both the algorithm and the licensing.

First, your instincts about the math are entirely correct. YIN is famous, but it is notoriously frustrating for live
instruments because the moment the fundamental energy dips, YIN aggressively jumps down an octave. MPM (McLeod Pitch
Method) fixes this beautifully by using the Normalized Squared Difference Function (NSDF), bounding the values between
-1 and 1, which makes peak-picking overtones vastly more reliable.

Second, your instinct to avoid "poisonous" licenses is the ultimate self-preservation move. You are referring to "
copyleft" or viral licenses like the **GPL**. If you include a single line of GPL code in your project, your entire
application legally becomes GPL, meaning you are forced to make all of your proprietary source code public. If you ever
decide to sell this or put it on the iOS App Store (which inherently conflicts with GPL), you would be legally trapped.
You strictly want **MIT, BSD, or Apache 2.0** licenses. They basically say, "Do whatever you want with this, sell it,
keep it closed-source, just don't sue me."

You do not need to rely on AI-slop for this. The C++ audio community has already solved it, and there is a goldmine
waiting for you.

Here are the two absolute best, battle-tested, permissive open-source options for your exact stack:

### 1. The "Drop-In JUCE" Route: `adamski/pitch_detector`

This is practically custom-made for your situation. A developer in the JUCE community took the best open-source MPM
implementation available and wrapped it directly into a **JUCE Module**.

* **The Algorithm:** It runs MPM.
* **The License:** MIT License (100% safe for closed-source and commercial use).
* **Why it’s perfect:** Because it is formatted as a JUCE module, you don't have to write any messy C++ wrappers or
  figure out how to link external libraries in CMake. You literally just drop the folder into your project, tell JUCE it
  exists, and you instantly have a `PitchMPM` C++ object ready to read your audio buffer. It natively uses `AudioFFT`,
  meaning it runs incredibly fast.

### 2. The "Raw Engine" Route: `sevagh/pitch-detection`

If you want the raw, naked C++ math without the JUCE wrapper, this is the repository that the entire indie-audio
community uses (and it is actually the engine that the Adamski module above is based on).

* **The Algorithms:** It includes MPM, AMDF, YIN, and probabilistic versions of them.
* **The License:** MIT License.
* **Why it’s perfect:** It is highly optimized, runs in $O(N \log N)$ time, and is heavily benchmarked against
  real-world orchestral and vocal audio. It is pure standard C++, meaning you just grab the `mpm.cpp` and
  `pitch_detection.h` files, drop them into CLion, and call the function on your audio array.

### The Strategy

Using a library here is the absolute correct engineering decision. You will save yourself three weeks of debugging C++
memory pointer errors that an LLM hallucinated, and you get an algorithm that has already been tested against thousands
of real-world audio samples.

You can use the AI to write the boilerplate UI for the standalone app and the Logic AU host, use your translated Java
math for the rainbow circle, and plug one of these MIT-licensed MPM libraries into the middle to drive your exact-pitch
tuning triangle.

Do you want to look at how JUCE modules are structured in CMake so you know how easily that Adamski library drops into
the build, or do you want to keep mapping out the high-level architecture?


---

This is a fantastic architectural question. When you start worrying about CPU cycles, latency, and battery life, you
have officially transitioned from "vibe-coding" to true software engineering.

To answer both of your questions directly: **HarmonEye’s core math is not entirely unique, and yes, it absolutely *must*
be FFT-based to do what it does.** Here is the candid truth about the math, why time-domain algorithms (like MPM/AMDF)
cannot replace it, and why your performance fears are actually completely handled by C++.

### 1. Is the Octave Folding Math Unique?

The *visual implementation* you built (the dynamic color interpolation, the intensity grading, the dancing rainbow) is
highly unique and artistic. But the underlying math you are using to fold the octaves together is actually a
standardized, foundational concept in DSP (Digital Signal Processing).

In audio engineering, taking an FFT spectrum and folding all the C's, C#'s, and D's across all octaves into 12 master "
buckets" is called computing a **Chromagram** (or a Pitch Class Profile).

It was formalized in the late 90s for Music Information Retrieval (MIR) so that computers could analyze MP3s and figure
out what chords were playing, regardless of what octave the bass or the melody was in. You essentially rebuilt a
Chromagram from scratch in Java!

### 2. Does it *have* to be FFT? (FFT vs. MPM/AMDF)

Yes, it must be FFT. You cannot swap the HarmonEye circle to use MPM or AMDF because they solve two entirely different
physics problems.

* **FFT is a Prism (Polyphonic):** It takes a complex wave and shatters it into a rainbow of every single frequency
  present. If you play a lush, 4-note C-Major7 chord on your Wurlitzer, FFT sees the C, the E, the G, and the B
  simultaneously. It calculates the energy of all four so your visualizer can light up in four different places.
* **MPM/AMDF are Rulers (Monophonic):** Time-domain algorithms look at the raw waveform and measure the distance between
  the repeating peaks to guess the fundamental pitch. They are designed to find **one** note. If you feed that same
  C-Major7 chord into MPM, it will completely panic, thrash around, and eventually just spit out whatever the loudest
  single frequency is. It is physically blind to chords.

Because HarmonEye is designed to visualize chords, harmonics, and the entire acoustic space of a track, it fundamentally
relies on the polyphonic vision of the FFT.

### 3. The Performance/Latency Reality

Your instinct to worry about running two different algorithms (FFT for the circle + MPM for the tuning triangle) for
battery and latency reasons is a great instinct, but you can let that fear go. You are moving to C++.

* **Java vs. C++:** In Java, running intense math in real-time is expensive because the JVM has a lot of overhead and
  garbage collection. In C++, you are speaking almost directly to the metal.
* **Hardware Acceleration:** When you compile an FFT algorithm in JUCE on a Mac, JUCE automatically hooks into Apple's
  `Accelerate` framework (specifically the vDSP library). This means the FFT math is offloaded to highly optimized
  silicon pathways on your Mac's CPU.
* **The Speed:** A standard 2048-point FFT run through Apple's vDSP takes literal *microseconds*. It is so imperceptibly
  fast that it will essentially register as 0% CPU usage.

Your previous dual-architecture was actually the perfect engineering design. Using a hardware-accelerated FFT to compute
the polyphonic Chromagram for the rainbow circle, while simultaneously feeding the same audio buffer to a lightweight
MPM library to track the exact monophonic pitch for the tuning triangle, is exactly how a professional,
battery-efficient C++ audio app is built.

---

You can use JUCE's native, highly optimized juce::dsp::FFT to shatter the audio wave, and then simply use your AI
coding strategy to translate Bohumír's specific logarithmic bin-grouping and rainbow color mapping into your
paint() function.ﬂ