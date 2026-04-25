## Prompt:
Do i have to apply a low-pass filter or downsampling would be enough?

I ported sevagh/pitch-detection to JUCE and it is already much better than adamski and Cycfi Q - it works as low as G1 rather than E2.

\---

' Low-Pass Filter: Apply a steep Butterworth low-pass filter (which you already built) at around 300 Hz to kill the higher harmonics that confuse the pitch tracker.

' Decimation (Downsampling): Downsample that specific audio buffer by a factor of 4 (e.g., from 44,100 Hz to 11,025 Hz).

' Run the Detection: Feed that downsampled buffer into the pitch detector. Because the sample rate is 4 times lower, a 1024-sample buffer now represents 92 milliseconds of time context. This easily captures frequencies all the way down to 20 Hz without increasing CPU load. (In fact, because you are processing fewer samples, it is vastly more efficient).

## Response:
### Confidence Level: High

> **Sources:** The Nyquist-Shannon Sampling Theorem; standard Digital Signal Processing principles for "Decimation" (e.g., _Discrete-Time Signal Processing_ by Oppenheim & Schafer).

Getting down to G1 just by porting `sevagh/pitch-detection` into your architecture is a massive win. That proves the pYIN algorithm is doing exactly the heavy lifting it was designed to do.

To answer your question directly: **You absolutely must apply the low-pass filter before downsampling.** You cannot skip it.

If you downsample without filtering the audio first, the pitch tracking will instantly break and perform much worse than before. Here is the physical and mathematical reality of why the filter is a mandatory step:

### 1\. The Anti-Aliasing Law (The DSP Requirement)

' When you downsample an audio stream by a factor of 4 (from **44,100 Hz** to **11,025 Hz**), the "speed limit" for frequencies in that buffer drops drastically. ' According to the Nyquist theorem, the maximum frequency an **11,025 Hz** sample rate can represent is exactly half: **5,512.5 Hz**. ' If your original 44.1 kHz buffer contains _any_ audio energy above **5,512.5 Hz** (like pick attack, fret buzz, or cymbal bleed) and you drop samples without filtering, those high frequencies do not magically disappear. They "fold back" into the low-frequency spectrum as harsh, inharmonic noise. ' This is called **aliasing**. It destroys the periodic shape of the waveform, causing the pYIN algorithm to panic. The low-pass filter acts as the required "anti-aliasing" shield.

### 2\. The Harmonic Decoupling (The Pitch Tracking Requirement)

' Beyond just preventing mathematical aliasing, setting that low-pass filter cutoff extremely low (around **300 Hz**) solves the octave jitter problem at the physical source. ' On a bass guitar, the second harmonic (the octave above the root note) is often physically louder than the fundamental frequency of the vibrating string. ' By brutally filtering out everything above 300 Hz, you intentionally silence the overtones of those E1 and A1 notes. You are effectively feeding the algorithm a pure, dark sine wave, leaving pYIN with zero opportunity to accidentally latch onto the wrong octave.

### The Order of Operations

In audio engineering, downsampling without a filter is just "dropping samples." Downsampling _with_ a filter is called **Decimation**, and it must happen in this exact order:

' Take the incoming 44.1/48 kHz audio buffer. ' Run it through your Butterworth low-pass filter. ' Throw away 3 out of every 4 samples to create the new, smaller buffer. ' Feed that decimated buffer into the `sevagh` pitch detector.

Are you planning to implement a ring buffer to collect enough of these downsampled blocks before triggering the pitch calculation, or does your current JUCE block size already give you enough temporal context at the lower sample rate?

