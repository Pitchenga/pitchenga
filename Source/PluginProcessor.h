#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <array>

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

    void getStateInformation (juce::MemoryBlock& destData) override { juce::ignoreUnused (destData); }
    void setStateInformation (const void* data, int sizeInBytes) override { juce::ignoreUnused (data, sizeInBytes); }

    static constexpr int numOctaves = 6;
    // 32768 is sufficient to hold the lowest octave buffers without overrun at 30fps
    static constexpr int fifoSize = 32768; 

    struct OctaveBuffer {
        juce::AbstractFifo fifo { fifoSize };
        std::vector<float> buffer;
        juce::dsp::IIR::Filter<float> lowpass;
        bool dropNext = false;

        OctaveBuffer() : buffer(fifoSize, 0.0f) {}
    };

    std::array<OctaveBuffer, numOctaves>& getOctaves() { return octaves; }

private:
    std::array<OctaveBuffer, numOctaves> octaves;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchengaAudioProcessor)
};
