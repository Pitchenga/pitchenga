#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>

// Include Cycfi Q's Pitch Detector
#include <q/pitch/pitch_detector.hpp>

#include <vector>
#include <array>
#include <memory>
#include <atomic>

class FastButterworth
{
    std::array<float, 7> inputSamples {0.0f};
    std::array<float, 7> outputSamples {0.0f};
    static constexpr float gainInv = 1.0f / 33.79723001f;
    const std::array<float, 7> inCoeffs = {1.0f, 6.0f, 15.0f, 20.0f, 15.0f, 6.0f, 1.0f};
    const std::array<float, 7> outCoeffs = {-0.0017509260f, 0.0f, -0.1141994251f, 0.0f, -0.7776959619f, 0.0f, 0.0f};

public:
    float processSample (float signal)
    {
        // Shift history buffers
        for (size_t i = 0; i < 6; ++i) {
            inputSamples[i] = inputSamples[i + 1];
            outputSamples[i] = outputSamples[i + 1];
        }

        inputSamples[6] = signal * gainInv;

        float sumIn = 0.0f;
        float sumOut = 0.0f;
        for (size_t i = 0; i < 7; ++i) {
            sumIn += inCoeffs[i] * inputSamples[i];
            sumOut += outCoeffs[i] * outputSamples[i];
        }

        outputSamples[6] = sumIn + sumOut;
        return outputSamples[6];
    }

    void reset() {
        inputSamples.fill(0.0f);
        outputSamples.fill(0.0f);
    }
};

class PitchengaAudioProcessor : public juce::AudioProcessor
{
public:
    PitchengaAudioProcessor();
    ~PitchengaAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int index) override { juce::ignoreUnused (index); }
    const juce::String getProgramName (int index) override { juce::ignoreUnused (index); return {}; }
    void changeProgramName (int index, const juce::String& newName) override { juce::ignoreUnused (index, newName); }

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Default size is used if no state is loaded
    int lastUIWidth = 700;
    int lastUIHeight = 730;

    static constexpr int numOctaves = 6;
    // 32768 is sufficient to hold the lowest octave buffers without overrun at 30fps
    static constexpr int fifoSize = 32768; 

    struct OctaveBuffer {
        juce::AbstractFifo fifo { fifoSize };
        std::vector<float> buffer;
        FastButterworth lowpass;
        bool dropNext = false;

        OctaveBuffer() : buffer(fifoSize, 0.0f) {}
    };

    std::array<OctaveBuffer, numOctaves>& getOctaves() { return octaves; }

    // This is the lock-free bridge to the GUI
    std::atomic<float> currentPitchHz { -1.0f };

private:
    std::array<OctaveBuffer, numOctaves> octaves;
    std::vector<float> monoBuffer;
    std::vector<float> nextStageBuffer;

    // Pitch detection buffers
    std::vector<float> pitchCircularBuffer;
    int pitchWritePos = 0;
    std::vector<float> pitchAnalysisBuffer;
    int samplesSinceLastPitchDetection = 0;

    std::unique_ptr<cycfi::q::pitch_detector> pitchDetector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchengaAudioProcessor)
};

