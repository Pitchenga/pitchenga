### Specification: Native macOS Desktop Audio Capture (ScreenCaptureKit)

#### 1\. Objective

Capture system audio natively on macOS 12.3+ without any third-party drivers, and seamlessly inject it into `PitchengaAudioProcessor`'s visualization matrix.

#### 2\. Dependencies & CMake Configuration

You must link the required Apple frameworks in your `CMakeLists.txt`.

```
# In CMakeLists.txt, inside target_link_libraries:
target_link_libraries(Pitchenga PRIVATE
    "-framework ScreenCaptureKit"
    "-framework CoreMedia"
    "-framework AVFoundation"
)
```

#### 3\. Architectural Strategy

We will create a standalone Objective-C++ module (`DesktopAudioCapture.h` and `DesktopAudioCapture.mm`). It will act exactly like your `SteamWorker` thread: it runs in the background, receives system audio from Apple's API, and pushes it into a lock-free `juce::AbstractFifo`. Your `processBlock` will pop this FIFO and mix it into the `monoData` buffer right alongside your microphone and external synthesizer.

#### 4\. Implementation Steps

##### Step 4.1: Permissions (Info.plist)

macOS requires explicit user consent to capture the screen/desktop audio. You must add the following entitlement to your standalone app's `.plist` file via CMake:

*   Key: `NSScreenCaptureUsageDescription`
*   Value: "Pitchenga needs screen capture access to visualize your desktop audio."

##### Step 4.2: The Objective-C++ Wrapper (`DesktopAudioCapture.mm`)

You will write a custom class that implements the `SCStreamOutput` protocol.

1.  **Get Shareable Content:** Call `[SCShareableContent getShareableContentWithCompletionHandler:]` to find the main desktop display.
2.  **Configure the Stream:**
    *   Create an `SCStreamConfiguration`.
    *   Set `capturesAudio = YES`.
    *   Set `excludesCurrentProcessAudio = YES` (**Crucial:** This prevents a feedback loop where Pitchenga captures its own output and deafens the user).
    *   Set the sample rate to match your `PitchengaAudioProcessor` sample rate (e.g., 44100).
3.  **Start the Stream:** Initialize the `SCStream` and call `[stream startCaptureWithCompletionHandler:]`.

##### Step 4.3: The Audio Callback (The Bridge)

When Apple's API captures a chunk of audio, it fires a callback containing a `CMSampleBuffer`. We need to extract the raw floats and push them to JUCE.

```
// Inside your SCStreamOutput delegate method:
- (void)stream:(SCStream *)stream didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer ofType:(SCStreamOutputType)type {
    if (type != SCStreamOutputTypeAudio) return;

    CMBlockBufferRef blockBuffer;
    AudioBufferList audioBufferList;
    
    // Extract the raw audio data from Apple's CMSampleBuffer
    CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(
        sampleBuffer, NULL, &audioBufferList, sizeof(audioBufferList),
        NULL, NULL, 0, &blockBuffer
    );

    // Get the float array (assuming Apple returns 32-bit floats)
    float* audioData = (float*)audioBufferList.mBuffers[0].mData;
    int numSamples = audioBufferList.mBuffers[0].mDataByteSize / sizeof(float);

    // Push audioData into a juce::AbstractFifo shared with PitchengaAudioProcessor
    cppCaptureInstance->pushAudioToFifo(audioData, numSamples);
    
    CFRelease(blockBuffer);
}
```

##### Step 4.4: Injecting into the Routing Matrix (`PluginProcessor.cpp`)

Now we update the routing matrix you built previously to include Stream E (Desktop Audio).

1.  Instantiate your `DesktopAudioCapture` class in the `PitchengaAudioProcessor`.
2.  Give it a `juce::AbstractFifo` and a `std::vector<float> desktopAudioBuffer` capable of holding about 8192 samples (since Apple's callback block sizes will not perfectly match JUCE's `processBlock` sizes).
3.  Inside `processBlock`:
    *   Pop whatever samples are available from the `desktopAudioFifo`.
    *   Mix them directly into your `monoData` buffer alongside `micBuffer` and `pluginOutputBuffer`.
