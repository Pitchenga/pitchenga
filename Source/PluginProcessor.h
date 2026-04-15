#pragma once

#include <JuceHeader.h>
#include <vector>

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
    void setCurrentProgram (int index) override {}
    const juce::String getProgramName (int index) override { return {}; }
    void changeProgramName (int index, const juce::String& newName) override {}

    void getStateInformation (juce::MemoryBlock& destData) override {}
    void setStateInformation (const void* data, int sizeInBytes) override {}

    // Lock-free FIFO access for the Editor
    static constexpr int fifoSize = 16384;
    juce::AbstractFifo& getFifo() { return fifo; }
    const std::vector<float>& getFifoBuffer() const { return fifoBuffer; }

private:
    juce::AbstractFifo fifo { fifoSize };
    std::vector<float> fifoBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchengaAudioProcessor)
};
