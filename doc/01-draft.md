System Role: You are an expert C++ Audio DSP Developer specializing in the JUCE Framework (v8) and modern C++20.

Context: I am building a dual-format (AU and Standalone) audio plugin called "Pitchenga". The CMakeLists.txt is
configured. The Source directory is currently empty.

The Task: Write the initial implementation for the four core files: PluginProcessor.h, PluginProcessor.cpp,
PluginEditor.h, and PluginEditor.cpp.

Architectural Requirements:

The Audio Thread (processBlock): >    * Intercept the incoming audio buffer and mix it down to mono if it is stereo.

Push the mono audio samples into a lock-free FIFO queue. Do not allocate memory, use locks, or run heavy math on the
audio thread.

The DSP/Math (FFT & Octave Folding):

Use juce::dsp::FFT with an exact order of 12 (size 4096) for high frequency resolution.

The GUI thread should pull from the lock-free FIFO, perform the FFT, and calculate a high-resolution Pitch Class
Profile.

CRITICAL: The visualizer uses 108 bins (9 bins per semitone, ).

The GUI Thread (PluginEditor):

Inherit from juce::Timer and tick at 30fps.

Use strictly juce::Graphics in the paint() method (no OpenGL).

Draw a dynamic, resizable UI. Draw a circular layout consisting of 60 segments (like a high-resolution clock face).

Map the energy of the 60 folded bins to the radius or opacity of these 60 segments, interpolating colors smoothly across
the spectrum based on the Java logic provided.

Output: Implement the C++ code for all four files. Prioritize thread safety (strict separation between audio and UI
threads) and idiomatic JUCE C++20 structure. Do not explain the CMake setup; just provide the C++ implementations.

Below is the legacy Java code from the original application. Translate this exact octave-folding math, binning logic,
and terminology into idiomatic, optimized C++ to process the FFT magnitude array.

Analysis: @MusicAnalyzer
Vizualization: @OpenGlCircularVisualizer @ColorFunction 

CRITICAL: use the legacy java code only as a reference. DO NOT blindly translate the java code into C++: implement a proper idiomatic C++ code.