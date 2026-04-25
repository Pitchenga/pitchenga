
The mini-DAW feature for loading external plugins.
It only works in standalone mode - no need for the DAW within a DAW complexity.
Catch: Both the incoming audio and the instrument to be routed to the visualizers, BUT also (configurably) mute the pass-through of the incoming audio - so that it does not always monitor my microphones to the speakers.

---

**The Plugin Hosting Engine**

*   We will implement a `juce::AudioPluginFormatManager`. This is JUCE's native librarian that scans the user's computer for installed AU and VST3 plugins.
*   We will use `juce::AudioPluginInstance` to actually load the chosen external instrument. This object behaves exactly like a mini audio processor living inside your main processor.
*   When the user selects a plugin, we instantiate it, pass it the current sample rate and block size, and hold it in a thread-safe `std::unique_ptr`.

**The Routing Matrix (Solving the Catch)** To ensure the visualizers see everything but your speakers do not feedback from the microphone, we will fundamentally change how audio flows through `processBlock`. Instead of a single straight pipe, we will create an internal mixing bus.

*   **Stream A (The Microphone):** We copy the raw incoming audio into a dedicated buffer.
*   **Stream B (The Instrument):** We create an empty buffer, hand it to the external `AudioPluginInstance` along with your MIDI data, and tell the external plugin to render its audio into it.
*   **Stream C (The Visualizer Feed):** We mathematically add Stream A and Stream B together into your existing `monoBuffer`. This buffer goes straight to your STFT and CQT engines, meaning the visualizer sees both the live microphone and the synthesizer perfectly blended.
*   **Stream D (The Speaker Output):** We create a dedicated output buffer. We copy Stream B (the synth) into it at 100% volume. We multiply Stream A (the microphone) by a user-controlled "Mic Monitor" toggle (1.0 or 0.0) and add it to the output. We then hand this final mixed buffer to the hardware speakers.

**The External UI Window**

*   External plugins come with their own graphical interfaces. We cannot easily draw them inside your heavily optimized OpenGL/CoreGraphics visualizer window.
*   Instead, we will spawn a separate, floating `juce::DocumentWindow` when the user clicks an "Open Instrument UI" button. JUCE will handle embedding the external AU/VST3 UI directly into that floating window.

**State Management**

*   When the user tweaks knobs on their loaded Serum or Kontakt instance, we need to save those settings so they recall when they reopen your app.
*   We will extract the raw memory block from the `AudioPluginInstance` and encode it into base64, storing it directly inside your existing XML settings file.

How does this blueprint look to you? We can start by drafting the UI controls to select and load a plugin, or we can start with the heavy DSP routing in the processor.

* * *
**MASTER IMPLEMENTATION SPECIFICATION: External Plugin Host & Custom Routing Matrix**

**Objective:**

Evolve a JUCE audio visualizer plugin into a "mini-DAW" by implementing the ability to host external VST3/AU instruments (in Standalone mode). Implement a custom audio routing matrix inside processBlock so that both the live microphone and the external instrument feed into the visualizer engines, but the live microphone can be independently muted from the physical speaker output to prevent feedback.

**Context & Existing Architecture:**

* The plugin currently takes stereo input, mixes it to a monoBuffer, and feeds it to a decimation cascade (lock-free AbstractFifo array) for background thread visualization (STFT, CQT, Pitch).
* The plugin receives juce::MidiBuffer messages in processBlock.

### **Phase 1: Core Plugin Hosting (PluginProcessor.h & .cpp)**

1. **Members:**
    * Include \<juce\_audio\_processors/juce\_audio\_processors.h\>.
    * Add juce::AudioPluginFormatManager formatManager;.
    * Add std::unique\_ptr\<juce::AudioPluginInstance\> externalPlugin;.
    * Add juce::AudioBuffer\<float\> pluginOutputBuffer; (to isolate the instrument's audio).
2. **Initialization (prepareToPlay & Constructor):**
    * In the constructor, call formatManager.addDefaultFormats(); so it knows how to handle VST3/AU.
    * In prepareToPlay, initialize pluginOutputBuffer to match the expected block size and output channels.
    * If externalPlugin is valid, call externalPlugin-\>prepareToPlay(sampleRate, samplesPerBlock);.
3. **Loading Mechanism:**
    * Create a public method void loadExternalPlugin(const juce::PluginDescription& description).
    * Use formatManager.createPluginInstanceAsync() to safely load the plugin on the message thread and assign it to externalPlugin. Once loaded, call its prepareToPlay.

### **Phase 2: The Audio Routing Matrix (processBlock)**

**CRITICAL:** We must manipulate the audio streams to separate "What the visualizer sees" from "What the speaker plays".

1. **Setup:**
    * Read a new setting settings.isMonitoringEnabled (boolean).
    * Ensure pluginOutputBuffer is sized correctly and cleared to 0.0f.
2. **Stream A (Microphone Intake):**
    * The main buffer currently holds the live microphone input. Copy this safely into a temporary juce::AudioBuffer\<float\> micBuffer to preserve it.
3. **Stream B (Instrument Processing):**
    * If externalPlugin is valid, call externalPlugin-\>processBlock(pluginOutputBuffer, midiMessages);.
    * *Note: The external plugin receives the incoming MIDI from the user's keyboard and renders audio into pluginOutputBuffer.*
4. **Stream C (The Visualizer Feed):**
    * The visualizer relies on monoData.
    * Mix down micBuffer into monoData (as it currently does).
    * Mix down pluginOutputBuffer into monoData (additive).
    * *Result: The decimation cascade and background math threads now visualize BOTH the microphone and the synthesizer seamlessly.*
5. **Stream D (The Speaker Output):**
    * Clear the main buffer completely (this erases the default microphone passthrough).
    * Add pluginOutputBuffer to the main buffer (the instrument always goes to the speakers).
    * If settings.isMonitoringEnabled \== true, add micBuffer back into the main buffer.
    * *Result: Feedback loop prevented. Users can play the synth silently, or sing along with it.*

### **Phase 3: External GUI Windowing (PluginEditor.h & .cpp)**

1. **UI Controls:**
    * Add a "Instrument" button (opens a juce::KnownPluginList scanner or file chooser).
    * Add a "Show" button.
    * Add a "Monitor" toggle button.
2. **Window Management:**
    * We cannot draw the external VST3 UI inside our CoreGraphics visualizer canvas.
    * When "Show" is clicked, check if externalPlugin-\>hasEditor() is true.
    * If true, spawn a separate, floating juce::DocumentWindow.
    * Call externalPlugin-\>createEditorIfNeeded() and set it as the content component of the DocumentWindow.
    * Ensure the DocumentWindow cleans up safely when closed so it doesn't leak or crash the main app.

### **Phase 4: State Preservation (getStateInformation / setStateInformation)**

1. **Save:**
    * If externalPlugin is loaded, extract its state using externalPlugin-\>getStateInformation(memoryBlock).
    * Convert the raw memory block to Base64 and save it in the main XML settings tree (e.g., \<ExternalPluginState base64="..." /\>).
2. **Restore:**
    * During load, read the Base64 string, decode it back into a juce::MemoryBlock, and call externalPlugin-\>setStateInformation(). *(Note: You will need to store the PluginDescription or File Path in the XML as well, so the app knows WHICH plugin to load before applying the state).*

**Constraints for the LLM:**

* Ensure thread safety when swapping or loading externalPlugin while processBlock is running (use juce::ScopedReadLock or similar locking around the plugin processing pointer).
* Do not remove any existing logic pertaining to the Math worker, decimation cascade, or STFT rendering.
* Implement only the code necessary to implement this routing and hosting logic.