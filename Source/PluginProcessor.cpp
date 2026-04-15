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
    juce::ignoreUnused (samplesPerBlock);

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

    // Mix down to mono
    std::vector<float> mono (static_cast<size_t>(numSamples), 0.0f);
    if (totalNumInputChannels == 1)
    {
        juce::FloatVectorOperations::copy (mono.data(), buffer.getReadPointer (0), numSamples);
    }
    else if (totalNumInputChannels >= 2)
    {
        auto* left = buffer.getReadPointer (0);
        auto* right = buffer.getReadPointer (1);
        juce::FloatVectorOperations::add (mono.data(), left, right, numSamples);
        juce::FloatVectorOperations::multiply (mono.data(), 0.5f, numSamples);
    }

    // Cascade multi-rate decimation
    std::vector<float> currentStage = std::move (mono);

    for (int oct = 0; oct < numOctaves; ++oct)
    {
        // 1. Push current stage samples to this octave's lock-free FIFO
        auto scope = octaves[oct].fifo.write (static_cast<int>(currentStage.size()));
        
        auto pushToFifo = [&](int fifoStart, int amount, int offset)
        {
            juce::FloatVectorOperations::copy (octaves[oct].buffer.data() + fifoStart, 
                                               currentStage.data() + offset, 
                                               amount);
        };

        if (scope.blockSize1 > 0) pushToFifo (scope.startIndex1, scope.blockSize1, 0);
        if (scope.blockSize2 > 0) pushToFifo (scope.startIndex2, scope.blockSize2, scope.blockSize1);

        // 2. Filter and Decimate by 2 for the next octave
        if (oct < numOctaves - 1)
        {
            std::vector<float> nextStage;
            nextStage.reserve (currentStage.size() / 2 + 1);
            
            for (float sample : currentStage)
            {
                float filtered = octaves[oct].lowpass.processSample (sample);
                if (! octaves[oct].dropNext)
                {
                    nextStage.push_back (filtered);
                }
                octaves[oct].dropNext = ! octaves[oct].dropNext;
            }
            
            currentStage = std::move (nextStage);
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
