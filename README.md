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

- **macOS:** Available as a Standalone `.app`, Audio Unit (AU), and VST3. _(Note: The current releases are not yet signed with an Apple Developer certificate. You will need to right-click and select "Open" to bypass Gatekeeper on the first launch)._
- **Windows:** Available as a Standalone `.exe` and VST3.
- **Linux:** Please compile from source using the instructions below.

### Building from Source

For Linux users, developers, and DSP enthusiasts, Pitchenga can be easily compiled using CMake.

- Clone the repository recursively to fetch the JUCE and Eigen submodules: `git clone --recursive https://github.com/yourusername/pitchenga.git`
- Navigate into the project directory: `cd pitchenga`
- Configure the build environment: `cmake -B build`
- Compile the project: `cmake --build build --config Release`
- The compiled Standalone and Plugin binaries will be located in the `build/Pitchenga_artefacts` directory.

### License

//fixme

