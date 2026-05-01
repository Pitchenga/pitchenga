#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <memory>
#include <vector>

class DesktopAudioCapture {
public:
    DesktopAudioCapture();
    ~DesktopAudioCapture();

    void start(double sampleRate);
    void stop();

    juce::AbstractFifo& getFifo() { return fifo; }
    std::vector<float>& getBuffer() { return ringBuffer; }

    void pushAudio(const float* data, int numSamples);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    static constexpr int ringBufferSize = 65536; // Larger buffer to handle bursty callbacks
    juce::AbstractFifo fifo{ringBufferSize};
    std::vector<float> ringBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesktopAudioCapture)
};
