#include "PluginProcessor.h"
#include "PluginEditor.h"

PitchengaAudioProcessor::PitchengaAudioProcessor()
    : AudioProcessor(
        BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    ) {}

PitchengaAudioProcessor::~PitchengaAudioProcessor() = default;

void PitchengaAudioProcessor::prepareToPlay(const double sampleRate, const int samplesPerBlock) {
    const size_t bufferSize = static_cast<size_t>(std::max(samplesPerBlock, 4096));
    monoBuffer.assign(bufferSize, 0.0f);
    nextStageBuffer.assign(bufferSize, 0.0f);

    // Reset decimation octaves for the visualizer
    for (size_t i = 0; i < numOctaves; ++i) {
        octaves[i].fifo.reset();
        std::ranges::fill(octaves[i].buffer, 0.0f);

        octaves[i].lowpass.reset();
        octaves[i].dropNext = false;
    }
}

void PitchengaAudioProcessor::releaseResources() {}

bool PitchengaAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet()) return false;

    return true;
}

void PitchengaAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    // static uint32_t lastProcessLog = 0;
    // uint32_t now = juce::Time::getMillisecondCounter();
    // if (now - lastProcessLog > 1000) {
    //     double playheadTime = 0.0;
    //     if (auto* ph = getPlayHead()) {
    //         if (auto pos = ph->getPosition()) {
    //             playheadTime = pos->getTimeInSeconds().orFallback(0.0);
    //         }
    //     }
    //     DBG("=== PROCESS BLOCK ===");
    //     DBG("DAW Playhead Time: " << playheadTime << "s");
    //     DBG("Block Size: " << buffer.getNumSamples());
    //     DBG("FIFO Free Space: " << octaves[0].fifo.getFreeSpace());
    //     lastProcessLog = now;
    // }
    // // --------------------------

    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) buffer.clear(i, 0, numSamples);

    if (numSamples <= 0) return;

    // Safety check: if host exceeds promised block size, we must skip to avoid OOB/Allocation
    if (static_cast<size_t>(numSamples) > monoBuffer.size()) return;

    // Mix down to mono into pre-allocated buffer
    float* monoData = monoBuffer.data();
    if (totalNumInputChannels == 1) {
        juce::FloatVectorOperations::copy(monoData, buffer.getReadPointer(0), numSamples);
    } else if (totalNumInputChannels >= 2) {
        auto* left = buffer.getReadPointer(0);
        auto* right = buffer.getReadPointer(1);
        juce::FloatVectorOperations::copy(monoData, left, numSamples);
        juce::FloatVectorOperations::add(monoData, right, numSamples);
        juce::FloatVectorOperations::multiply(monoData, 0.5f, numSamples);
    }

    // Cascade multi-rate decimation using pointer swapping
    float* currentStageData = monoData;
    float* nextStageData = nextStageBuffer.data();
    int currentStageSize = numSamples;

    // NEW: Enforce Temporal Integrity across the decimation cascade.
    // If the UI thread falls behind and Octave 0 fills up, we MUST drop the audio for ALL octaves.
    // Otherwise, lower octaves secretly record seconds of audio into the past, permanently desyncing the spectrum.
    const bool fifoOverflow = (octaves[0].fifo.getFreeSpace() < currentStageSize);

    for (size_t oct = 0; oct < numOctaves; ++oct) {
        // Push current stage samples to this octave's lock-free FIFO (ONLY if we aren't overflowing)
        if (!fifoOverflow) {
            const auto scope = octaves[oct].fifo.write(currentStageSize);

            if (scope.blockSize1 > 0)
                juce::FloatVectorOperations::copy(
                    octaves[oct].buffer.data() + static_cast<size_t>(scope.startIndex1),
                    currentStageData,
                    scope.blockSize1
                );

            if (scope.blockSize2 > 0)
                juce::FloatVectorOperations::copy(
                    octaves[oct].buffer.data() + static_cast<size_t>(scope.startIndex2),
                    currentStageData + scope.blockSize1,
                    scope.blockSize2
                );
        }

        // Filter and Decimate by 2 for the next octave
        // (We ALWAYS run this math even during overflow to keep the Butterworth filter states stable)
        if (oct < numOctaves - 1) {
            int nextIdx = 0;
            for (int i = 0; i < currentStageSize; ++i) {
                const float filtered = octaves[oct].lowpass.processSample(currentStageData[i]);
                if (!octaves[oct].dropNext) {
                    nextStageData[nextIdx++] = filtered;
                }
                octaves[oct].dropNext = !octaves[oct].dropNext;
            }

            // Swap pointers for the next octave iteration
            float* temp = currentStageData;
            currentStageData = nextStageData;
            nextStageData = temp;
            currentStageSize = nextIdx;
        }
    }
}

void PitchengaAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    // Convert XML to binary for the host to save
    const juce::XmlElement xml = uiSettings.createXml();
    copyXmlToBinary(xml, destData);
}

void PitchengaAudioProcessor::setStateInformation(const void* data, const int sizeInBytes) {
    // Get the XML from the binary data provided by the host
    const std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && uiSettings.loadFromXml(*xmlState)) {
        // If the UI is currently open, tell the Editor to resize
        if (auto* editor = getActiveEditor()) {
            editor->setSize(uiSettings.lastUIWidth, uiSettings.lastUIHeight);
            if (auto* pitchengaEditor = dynamic_cast<PitchengaAudioProcessorEditor*>(editor)) {
                pitchengaEditor->updateVisibilityFromState();
            }
        }
    }
}

juce::AudioProcessorEditor* PitchengaAudioProcessor::createEditor() {
    return new PitchengaAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new PitchengaAudioProcessor();
}