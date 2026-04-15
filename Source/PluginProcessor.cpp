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
    juce::ignoreUnused (sampleRate, samplesPerBlock);
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
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const int numSamples = buffer.getNumSamples();

    // Mono Mixdown and Push to lock-free FIFO
    const auto scope = fifo.write (numSamples);

    auto pushToFifo = [&](int fifoStart, int amount, int bufferOffset)
    {
        float* dest = fifoBuffer.data() + static_cast<size_t> (fifoStart);

        if (totalNumInputChannels == 1)
        {
            juce::FloatVectorOperations::copy (dest, buffer.getReadPointer (0, bufferOffset), amount);
        }
        else if (totalNumInputChannels >= 2)
        {
            auto* left = buffer.getReadPointer (0, bufferOffset);
            auto* right = buffer.getReadPointer (1, bufferOffset);

            juce::FloatVectorOperations::multiply (dest, left, 0.5f, amount);
            juce::FloatVectorOperations::addWithMultiply (dest, right, 0.5f, amount);
        }
    };

    if (scope.blockSize1 > 0) pushToFifo (scope.startIndex1, scope.blockSize1, 0);
    if (scope.blockSize2 > 0) pushToFifo (scope.startIndex2, scope.blockSize2, scope.blockSize1);
}

juce::AudioProcessorEditor* PitchengaAudioProcessor::createEditor()
{
    return new PitchengaAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PitchengaAudioProcessor();
}
