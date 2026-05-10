# Pitchenga

**See your sound: the perfect pitch superpower for mere mortals.**

- Pitchenga is a high-precision, real-time harmonic visualizer - AU/VST plugin and standalone app. Designed to take the
  guesswork out of intonation, transcription, and sound design, it acts as a bionic ear — giving you artificial
  chromesthesia that tells you exactly what is happening in your audio.
- Whether you are checking your vocal pitch, tuning a tricky synth patch, or transcribing a dense chord progression,
  Pitchenga provides razor-sharp visual clarity.
- [Watch Pitchenga in action on YouTube](https://www.youtube.com/watch?v=oEOlSGb62Hw)

### Core Features

- **The Bionic Ear:** A true visual crutch for perfect pitch. Stop guessing what note your voice or instrument is
  hitting. Pitchenga provides instantaneous, mathematically accurate visual feedback across a continuous chromatic
  timeline.
- **The Ultimate Transcription Tool:** Reverse-engineer complex chords, fast solos, and dense harmonies. Pitchenga is
  the perfect companion for transcribing music, allowing you to visually decode exactly which notes are being played in
  any piece of audio without relying solely on your ear.
- **Surgical Note Isolation:** Standard spectrograms often blur adjacent notes together or crush quieter harmonics.
  Pitchenga utilizes a custom Topographic Prominence Extraction (Difference-of-Gaussians) to surgically isolate notes.
  If you play a dense chord or two adjacent semitones, both are drawn with distinct clarity.
- **Versatile Analyzer:** Pitchenga easily doubles as a traditional spectrum analyzer. Simply bypass the custom harmonic
  sculpting to view the raw, unprocessed audio data exactly like in standard studio tools.
- **Precision Strobe Tuner:** Dial in your instruments with microscopic accuracy. The built-in strobe tuning interface
  provides immediate, highly sensitive visual feedback for perfect intonation and fast microtonal adjustments.
- **Desktop Audio Capture (macOS):** Seamlessly intercept your system audio to instantly visualize tracks played
  directly from your browser or media player.

### Standalone Instrument Support

For quick practice sessions, you do not need to boot up a heavy DAW.
In Standalone mode, Pitchenga can host a VST3 or AU software instrument.
A custom internal routing matrix allows you to blend any live audio input — such as a microphone, guitar, or line-in —
with your software synth, complete with independent monitor controls to prevent feedback while you perform.

### Installation

Pre-built binaries are provided in the [Releases](https://github.com/pitchenga/pitchenga/releases) section.

- **macOS:** Available as a Standalone application, Audio Unit (AU Component), and VST3.
  - [Download latest release](https://github.com/pitchenga/pitchenga/releases/latest/download/Pitchenga.pkg)

- **Windows:** Available as a Standalone application and VST3.
  - [Download latest release](https://github.com/pitchenga/pitchenga/releases/latest/download/Pitchenga-Windows-Installer.exe)

> **Note:** If Windows Defender SmartScreen blocks the application from running,
> click **"More info"** and then select **"Run anyway"**.
>
> If you receive an error about **"MSVCP140.dll not found"**, you may need to install
> the [Microsoft Visual C++ Redistributable](https://aka.ms/vs/17/release/vc_redist.x64.exe).
>

- **Linux** //fixme
```
sudo apt-get install libasound2-dev libjack-dev
```

### Licenses and Acknowledgments

All original source code is released under the BSD 3-Clause License.
See the `LICENSE` file in the root of this repository for full details.

This project relies on third-party libraries and adapted code:

- **JUCE Framework:** Proprietary JUCE 8 Starter EULA.
- **Open-Source Dependencies:** All other libraries in the `/libs` directory are governed by their own respective
  open-source licenses. Please refer to the individual `LICENSE` files provided within each submodule's directory for
  their exact terms and copyright notices.
- **[HarmonEye](https://github.com/bzamecnik/harmoneye/tree/master/HarmonEye):** The "Eye" visualization and the CQT
  harmonic analysis logic were ported to C++ and evolved from the original Java-based HarmonEye project. Copyright (c)
  2012-2014 Bohumir Zamecnik. MIT License.
- **[Sevagh pitch-detection](https://github.com/sevagh/pitch-detection):** The pitch detector module was ported to JUCE
  from Sevag Hanssian's original MPM algorithm implementation. Copyright (c) 2018 Sevag Hanssian. MIT License.
