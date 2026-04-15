#include "PluginProcessor.h"
#include "PluginEditor.h"

PitchengaAudioProcessor::PitchengaAudioProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

PitchengaAudioProcessor::~PitchengaAudioProcessor() {}

void PitchengaAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    monoBuffer.resize (static_cast<size_t>(samplesPerBlock));
    nextStageBuffer.resize (static_cast<size_t>(samplesPerBlock));
    
    for (int i = 0; i < numOctaves; ++i)
    {
        octaves[i].fifo.reset();
        std::fill (octaves[i].buffer.begin(), octaves[i].buffer.end(), 0.0f);
        
        double currentSR = sampleRate / std::pow(2.0, i);
        // Half-band filter (cutoff at Nyquist/2 = currentSR/4) to prevent aliasing before downsampling
        octaves[i].lowpass.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass (currentSR, currentSR / 4.0);
        octaves[i].lowpass.reset();
        octaves[i].dropNext = false;
    }
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
    if (numSamples == 0) return;

    if (monoBuffer.size() < static_cast<size_t>(numSamples))
        monoBuffer.resize (static_cast<size_t>(numSamples));

    // Mix down to mono
    if (totalNumInputChannels == 1)
    {
        juce::FloatVectorOperations::copy (monoBuffer.data(), buffer.getReadPointer (0), numSamples);
    }
    else if (totalNumInputChannels >= 2)
    {
        auto* left = buffer.getReadPointer (0);
        auto* right = buffer.getReadPointer (1);
        juce::FloatVectorOperations::copy (monoBuffer.data(), left, numSamples);
        juce::FloatVectorOperations::add (monoBuffer.data(), right, numSamples);
        juce::FloatVectorOperations::multiply (monoBuffer.data(), 0.5f, numSamples);
    }

    // Cascade multi-rate decimation
    float* currentStageData = monoBuffer.data();
    int currentStageSize = numSamples;

    for (int oct = 0; oct < numOctaves; ++oct)
    {
        // 1. Push current stage samples to this octave's lock-free FIFO
        auto scope = octaves[oct].fifo.write (currentStageSize);
        
        auto pushToFifo = [&](int fifoStart, int amount, int offset)
        {
            juce::FloatVectorOperations::copy (octaves[oct].buffer.data() + fifoStart, 
                                               currentStageData + offset, 
                                               amount);
        };

        if (scope.blockSize1 > 0) pushToFifo (scope.startIndex1, scope.blockSize1, 0);
        if (scope.blockSize2 > 0) pushToFifo (scope.startIndex2, scope.blockSize2, scope.blockSize1);

        // 2. Filter and Decimate by 2 for the next octave
        if (oct < numOctaves - 1)
        {
            if (nextStageBuffer.size() < static_cast<size_t>(currentStageSize))
                nextStageBuffer.resize (static_cast<size_t>(currentStageSize));

            int nextIdx = 0;
            for (int i = 0; i < currentStageSize; ++i)
            {
                float filtered = octaves[oct].lowpass.processSample (currentStageData[i]);
                if (! octaves[oct].dropNext)
                {
                    nextStageBuffer[static_cast<size_t>(nextIdx++)] = filtered;
                }
                octaves[oct].dropNext = ! octaves[oct].dropNext;
            }
            
            // Swap buffers for the next iteration
            if (currentStageData == monoBuffer.data())
            {
                std::swap (monoBuffer, nextStageBuffer);
                currentStageData = monoBuffer.data();
            }
            else
            {
                std::swap (nextStageBuffer, monoBuffer);
                currentStageData = monoBuffer.data();
            }
            currentStageSize = nextIdx;
        }
    }
}

juce::AudioProcessorEditor* PitchengaAudioProcessor::createEditor()
{
    return new PitchengaAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PitchengaAudioProcessor();
}
