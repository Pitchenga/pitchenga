# Pitchenga

**See your sound: the "perfect pitch" superpower for mere mortals.**

- Pitchenga is a high-precision, zero-latency harmonic visualizer. Designed to take the guesswork out of intonation, transcription, and sound design, it acts as a bionic ear that tells you exactly what is happening in your audio in real-time.

- Whether you are checking your vocal pitch, tuning a tricky synth patch, or transcribing a dense chord progression, Pitchenga provides razor-sharp visual clarity without melting your CPU.

- [Watch Pitchenga in action on YouTube](//fixme)

### Core Features

- **The Bionic Ear:** A true visual crutch for perfect pitch. Stop guessing what note you or your synth is hitting. Pitchenga provides instantaneous, mathematically accurate visual feedback across a continuous chromatic timeline.
- **The Ultimate Transcription Tool:** Reverse-engineer complex chords, fast solos, and dense harmonies. Pitchenga is the perfect companion for transcribing music, allowing you to visually decode exactly which notes are being played in any piece of audio without relying solely on your ear.
- **Surgical Note Isolation:** Standard spectrograms often blur adjacent notes together or crush quieter harmonics. Pitchenga utilizes a custom Topographic Prominence Extractor (Difference-of-Gaussians) to surgically isolate notes. If you play a dense chord or two adjacent semitones, both are drawn with distinct clarity.
- **Optimized Smooth Rendering:** Visuals are decoupled onto a dedicated 48Hz background math thread. Combined with advanced magnitude smoothing to eliminate flickering, Pitchenga delivers a buttery-smooth, jitter-free scrolling spectrogram that keeps your CPU usage incredibly low.

### Standalone Instrument Support

For quick practice sessions, you do not need to boot up a heavy DAW.
In Standalone mode, Pitchenga can host a VST3 or AU software instrument.
A custom internal routing matrix allows you to blend your live microphone with your software synth, complete with independent monitor controls to prevent feedback while you play and sing.

### Downloads & Installation

Pre-built binaries are provided in the [Releases](fixme) section.

- **macOS:** Available as a Standalone `.app`, Audio Unit (AU), and VST3. 
> Note: The current releases are not yet signed with an Apple Developer certificate. You will need to right-click and select "Open" to bypass Gatekeeper on the first launch.
- **Windows:** Available as a Standalone `.exe` and VST3.

### Licenses and acknowledgments

All original source code is released under the BSD 3-Clause License.
See the `LICENSE` file in the root of this repository for full details.

This project relies on third-party libraries located in the /libs directory:

- JUCE Framework: Proprietary JUCE 8 Starter EULA.
- Open-Source Dependencies: All other libraries in the /libs directory are governed by their own respective open-source licenses. Please refer to the individual LICENSE files provided within each submodule's directory for their exact terms and copyright notices.
- The pitch detector module is a custom JUCE port of [Sevagh's](https://github.com/sevagh/pitch-detection) original MPM algorithm implementation. Copyright (c) 2018 Sevag Hanssian. MIT License.
- The "Eye" visualization and CQT harmonic analysis logic were ported from Java to C++ from the original [HaronEye](https://github.com/bzamecnik/harmoneye/tree/master/HarmonEye) project.  Copyright (c) 2012-2014 - Bohumir Zamecnik. MIT License.
