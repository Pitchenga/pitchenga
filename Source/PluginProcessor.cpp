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
    const size_t bufferSize = static_cast<size_t> (std::max (samplesPerBlock, 4096));
    monoBuffer.assign (bufferSize, 0.0f);
    nextStageBuffer.assign (bufferSize, 0.0f);

    pitchCircularBuffer.assign (8192, 0.0f);
    pitchAnalysisBuffer.assign (4096, 0.0f);
    pitchWritePos = 0;
    samplesSinceLastPitchDetection = 0;

    // Reset decimation octaves for the visualizer
    for (size_t i = 0; i < numOctaves; ++i)
    {
        octaves[i].fifo.reset();
        std::fill (octaves[i].buffer.begin(), octaves[i].buffer.end(), 0.0f);

        octaves[i].lowpass.reset();
        octaves[i].dropNext = false;
    }

    pitchDetector = std::make_unique<adamski::PitchMPM>(static_cast<int>(sampleRate), 4096);
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
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);

    if (numSamples <= 0) return;

    // Safety check: if host exceeds promised block size, we must skip to avoid OOB/Allocation
    if (static_cast<size_t>(numSamples) > monoBuffer.size()) return;

    // Mix down to mono into pre-allocated buffer
    float* monoData = monoBuffer.data();
    if (totalNumInputChannels == 1)
    {
        juce::FloatVectorOperations::copy (monoData, buffer.getReadPointer (0), numSamples);
    }
    else if (totalNumInputChannels >= 2)
    {
        auto* left = buffer.getReadPointer (0);
        auto* right = buffer.getReadPointer (1);
        juce::FloatVectorOperations::copy (monoData, left, numSamples);
        juce::FloatVectorOperations::add (monoData, right, numSamples);
        juce::FloatVectorOperations::multiply (monoData, 0.5f, numSamples);
    }

    // --- MPM Pitch Detection ---
    for (int i = 0; i < numSamples; ++i) {
        pitchCircularBuffer[static_cast<size_t>(pitchWritePos)] = monoData[i];
        pitchWritePos = (pitchWritePos + 1) % 8192;
    }
    samplesSinceLastPitchDetection += numSamples;

    // Run MPM if we have 1024 new samples
    if (samplesSinceLastPitchDetection >= 1024 && pitchDetector != nullptr)
    {
        samplesSinceLastPitchDetection = 0;

        // Extract the most recent 4096 samples from the ring buffer
        int readPos = (pitchWritePos - 4096 + 8192) % 8192;
        for (int i = 0; i < 4096; ++i) {
            pitchAnalysisBuffer[static_cast<size_t>(i)] = pitchCircularBuffer[static_cast<size_t>((readPos + i) % 8192)];
        }

        // This forces weak fundamentals above the MPM clarity threshold.
        juce::FloatVectorOperations::multiply (pitchAnalysisBuffer.data(), 12.0f, 4096);

        float detectedPitch = pitchDetector->getPitch (pitchAnalysisBuffer.data());
        currentPitchHz.store (detectedPitch, std::memory_order_relaxed);
    }

    // Cascade multi-rate decimation using pointer swapping
    float* currentStageData = monoData;
    float* nextStageData = nextStageBuffer.data();
    int currentStageSize = numSamples;

    for (size_t oct = 0; oct < numOctaves; ++oct)
    {
        // 1. Push current stage samples to this octave's lock-free FIFO
        auto scope = octaves[oct].fifo.write (currentStageSize);
        
        if (scope.blockSize1 > 0) 
            juce::FloatVectorOperations::copy (octaves[oct].buffer.data() + static_cast<size_t>(scope.startIndex1), 
                                               currentStageData, 
                                               scope.blockSize1);
                                               
        if (scope.blockSize2 > 0) 
            juce::FloatVectorOperations::copy (octaves[oct].buffer.data() + static_cast<size_t>(scope.startIndex2), 
                                               currentStageData + scope.blockSize1, 
                                               scope.blockSize2);

        // 2. Filter and Decimate by 2 for the next octave
        if (oct < numOctaves - 1)
        {
            int nextIdx = 0;
            for (int i = 0; i < currentStageSize; ++i)
            {
                float filtered = octaves[oct].lowpass.processSample (currentStageData[i]);
                if (! octaves[oct].dropNext)
                {
                    nextStageData[nextIdx++] = filtered;
                }
                octaves[oct].dropNext = ! octaves[oct].dropNext;
            }
            
            // Swap pointers for the next octave iteration
            float* temp = currentStageData;
            currentStageData = nextStageData;
            nextStageData = temp;
            currentStageSize = nextIdx;
        }
    }
}

void PitchengaAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Create an XML element to store settings
    juce::XmlElement xml ("PITCHENGA_SETTINGS");

    // Add width and height as attributes
    xml.setAttribute ("uiWidth", lastUIWidth);
    xml.setAttribute ("uiHeight", lastUIHeight);

    // Convert XML to binary for the host to save
    copyXmlToBinary (xml, destData);
}

void PitchengaAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Get the XML from the binary data provided by the host
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
    {
        // Only if the tag matches our expected name
        if (xmlState->hasTagName ("PITCHENGA_SETTINGS"))
        {
            // Update the processor variables.
            // If the attributes don't exist, it keeps the defaults initialized in .h
            lastUIWidth  = xmlState->getIntAttribute ("uiWidth", lastUIWidth);
            lastUIHeight = xmlState->getIntAttribute ("uiHeight", lastUIHeight);

            // If the UI is currently open, tell the Editor to resize
            if (auto* editor = getActiveEditor())
                editor->setSize (lastUIWidth, lastUIHeight);
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
