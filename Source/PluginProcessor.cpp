#include "PluginProcessor.h"
#include "PluginEditor.h"

PitchengaAudioProcessor::PitchengaAudioProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
    fifoBuffer.resize (fifoSize);
}

PitchengaAudioProcessor::~PitchengaAudioProcessor() {}

void PitchengaAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    fifo.reset();
    std::fill (fifoBuffer.begin(), fifoBuffer.end(), 0.0f);
}

void PitchengaAudioProcessor::releaseResources() {}

bool PitchengaAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void PitchengaAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const int numSamples = buffer.getNumSamples();

    // Mono Mixdown and Push to lock-free FIFO
    auto [start1, size1, start2, size2] = fifo.write (numSamples);

    auto pushToFifo = [&](int fifoStart, int amount, int bufferOffset)
    {
        if (totalNumInputChannels == 1)
        {
            const float* reader = buffer.getReadPointer (0, bufferOffset);
            std::copy (reader, reader + amount, fifoBuffer.data() + fifoStart);
        }
        else if (totalNumInputChannels >= 2)
        {
            const float* left = buffer.getReadPointer (0, bufferOffset);
            const float* right = buffer.getReadPointer (1, bufferOffset);
            for (int i = 0; i < amount; ++i)
                fifoBuffer[fifoStart + i] = (left[i] + right[i]) * 0.5f;
        }
    };

    if (size1 > 0) pushToFifo (start1, size1, 0);
    if (size2 > 0) pushToFifo (start2, size2, size1);

    fifo.finishedWrite (size1 + size2);
}

juce::AudioProcessorEditor* PitchengaAudioProcessor::createEditor()
{
    return new PitchengaAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PitchengaAudioProcessor();
}
