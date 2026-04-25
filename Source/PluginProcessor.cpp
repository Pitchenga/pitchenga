#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Util.h"

PitchengaAudioProcessor::PitchengaAudioProcessor()
    : AudioProcessor(
        BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    ) {
    juce::addDefaultFormatsToManager(formatManager);
    loadDefaultSettings();
}

PitchengaAudioProcessor::~PitchengaAudioProcessor() = default;

void PitchengaAudioProcessor::loadDefaultSettings() {
    // Attempt to load settings-default.xml from the Source directory
    const juce::File defaultSettingsFile(juce::File(__FILE__).getSiblingFile("settings-default.xml"));
    
    if (auto xml = juce::XmlDocument::parse(defaultSettingsFile)) {
        // We need the helper method from Control.cpp or to duplicate the tag logic. 
        // For simplicity and adherence to existing patterns, we'll assume the tag name 
        // is handled correctly by loadFromXml which checks the build profile.
        settings.loadFromXml(*xml);
    }
}

void PitchengaAudioProcessor::prepareToPlay(const double sampleRate, const int samplesPerBlock) {
    const size_t bufferSize = static_cast<size_t>(std::max(samplesPerBlock, 4096));
    monoBuffer.assign(bufferSize, 0.0f);
    nextStageBuffer.assign(bufferSize, 0.0f);

    agcLeft.reset();
    agcRight.reset();

    // Reset decimation octaves for the visualizer
    for (size_t i = 0; i < numOctaves; ++i) {
        octaves[i].fifo.reset();
        std::ranges::fill(octaves[i].buffer, 0.0f);

        octaves[i].lowpass.reset();
        octaves[i].dropNext = false;
    }

    // Plugin Hosting
    const juce::ScopedLock lock(pluginLock);
    
    const int numOutputChannels = getTotalNumOutputChannels();
    pluginOutputBuffer.setSize(numOutputChannels, samplesPerBlock);
    micBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);

    if (externalPlugin != nullptr) {
        externalPlugin->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

void PitchengaAudioProcessor::releaseResources() {
    const juce::ScopedLock lock(pluginLock);
    if (externalPlugin != nullptr) {
        externalPlugin->releaseResources();
    }
}

bool PitchengaAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet()) return false;

    return true;
}

void PitchengaAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    juce::ScopedNoDenormals noDenormals;

    if (numSamples <= 0) return;

    // Safety check: if host exceeds promised block size, we must skip to avoid OOB/Allocation
    if (static_cast<size_t>(numSamples) > monoBuffer.size()) return;

    // --- Audio Routing Matrix ---
    const juce::ScopedLock lock(pluginLock);

    // Stream A: Microphone Intake
    micBuffer.setSize(totalNumInputChannels, numSamples, false, false, false);
    for (int ch = 0; ch < totalNumInputChannels; ++ch) {
        micBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);
    }


    // Stream B: Instrument Processing
    pluginOutputBuffer.setSize(totalNumOutputChannels, numSamples, false, false, false);
    pluginOutputBuffer.clear();
    
    if (externalPlugin != nullptr) {
        externalPlugin->processBlock(pluginOutputBuffer, midiMessages);
    }

    // Stream C: The Visualizer Feed (Blended Mic + Instrument)
    // We use monoBuffer as the destination for visualizers
    float* monoData = monoBuffer.data();
    std::fill(monoBuffer.begin(), monoBuffer.end(), 0.0f);

    // Mix Mic into monoData
    if (totalNumInputChannels == 1) {
        juce::FloatVectorOperations::copy(monoData, micBuffer.getReadPointer(0), numSamples);
    } else if (totalNumInputChannels >= 2) {
        juce::FloatVectorOperations::copy(monoData, micBuffer.getReadPointer(0), numSamples);
        juce::FloatVectorOperations::add(monoData, micBuffer.getReadPointer(1), numSamples);
        juce::FloatVectorOperations::multiply(monoData, 0.5f, numSamples);
    }

    // Mix Instrument into monoData (additive)
    if (totalNumOutputChannels == 1) {
        juce::FloatVectorOperations::add(monoData, pluginOutputBuffer.getReadPointer(0), numSamples);
    } else if (totalNumOutputChannels >= 2) {
        // Average the stereo output for the mono visualizer
        for (int i = 0; i < numSamples; ++i) {
            monoData[i] += (pluginOutputBuffer.getReadPointer(0)[i] + pluginOutputBuffer.getReadPointer(1)[i]) * 0.5f;
        }
    }

    // Stream D: The Speaker Output
    buffer.clear();
    
    // Instrument always goes to speakers
    for (int ch = 0; ch < totalNumOutputChannels; ++ch) {
        buffer.addFrom(ch, 0, pluginOutputBuffer, ch % pluginOutputBuffer.getNumChannels(), 0, numSamples);
    }

    // Conditional input monitoring
    if (settings.isEarEnabled) {
        for (int ch = 0; ch < totalNumOutputChannels; ++ch) {
            buffer.addFrom(ch, 0, micBuffer, ch % micBuffer.getNumChannels(), 0, numSamples);
        }
    }

    // Cascade multi-rate decimation using pointer swapping (Visualizer pipeline continues normally)
    float* currentStageData = monoData;
    float* nextStageData = nextStageBuffer.data();
    int currentStageSize = numSamples;

    const bool fifoOverflow = (octaves[0].fifo.getFreeSpace() < currentStageSize);

    for (size_t oct = 0; oct < numOctaves; ++oct) {
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

        if (oct < numOctaves - 1) {
            int nextIdx = 0;
            for (int i = 0; i < currentStageSize; ++i) {
                const float filtered = octaves[oct].lowpass.processSample(currentStageData[i]);
                if (!octaves[oct].dropNext) {
                    nextStageData[nextIdx++] = filtered;
                }
                octaves[oct].dropNext = !octaves[oct].dropNext;
            }

            float* temp = currentStageData;
            currentStageData = nextStageData;
            nextStageData = temp;
            currentStageSize = nextIdx;
        }
    }
}

void PitchengaAudioProcessor::loadExternalPlugin(const juce::PluginDescription& description) {
    juce::String errorMessage;
    const double sampleRate = getSampleRate();
    const int blockSize = getBlockSize();
    
    // Create the plugin instance with host's current settings or defaults if not yet prepared
    auto instance = formatManager.createPluginInstance(description, sampleRate > 0 ? sampleRate : 44100.0, blockSize > 0 ? blockSize : 512, errorMessage);
    
    if (instance != nullptr) {
        // Safe UI cleanup: notify the editor to close any open plugin window before we replace the instance
        if (onPluginAboutToBeDeleted) {
            onPluginAboutToBeDeleted();
        }

        const juce::ScopedLock lock(pluginLock);
        
        // Ensure the new instance is prepared if we have valid host settings
        if (sampleRate > 0 && blockSize > 0) {
            instance->prepareToPlay(sampleRate, blockSize);
        }
        
        externalPlugin = std::move(instance);
        
        if (onPluginLoaded) {
            juce::MessageManager::callAsync([this] { if (onPluginLoaded) onPluginLoaded(); });
        }
    } else {
        Util::debug("Failed to load plugin: " + errorMessage);
    }
}

void PitchengaAudioProcessor::rescanPlugins() {
    if (onRescanPlugins) {
        onRescanPlugins();
    }
}

void PitchengaAudioProcessor::openPluginBrowser() {
    if (onOpenPluginBrowser) {
        onOpenPluginBrowser();
    }
}

void PitchengaAudioProcessor::showExternalPluginEditor() {
    if (onShowExternalPluginEditor) {
        onShowExternalPluginEditor();
    }
}

juce::AudioProcessorEditor* PitchengaAudioProcessor::createExternalPluginEditor() {
    const juce::ScopedLock lock(pluginLock);
    if (externalPlugin != nullptr) {
        return externalPlugin->createEditorIfNeeded();
    }
    return nullptr;
}

void PitchengaAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    const juce::ScopedLock lock(pluginLock);
    
    settings.externalPluginDescriptionXml = {};
    settings.externalPluginStateBase64 = {};

    if (externalPlugin != nullptr) {
        juce::MemoryBlock pluginState;
        externalPlugin->getStateInformation(pluginState);
        settings.externalPluginStateBase64 = pluginState.toBase64Encoding();
        
        auto pluginDescription = externalPlugin->getPluginDescription();
        if (auto pluginDescriptionXml = pluginDescription.createXml()) {
            settings.externalPluginDescriptionXml = pluginDescriptionXml->toString();
        }
    }

    juce::XmlElement xml = settings.createXml();
    copyXmlToBinary(xml, destData);
}

void PitchengaAudioProcessor::setStateInformation(const void* data, const int sizeInBytes) {
    const std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && settings.loadFromXml(*xmlState)) {
        if (auto* editor = getActiveEditor()) {
            editor->setSize(settings.lastUIWidth, settings.lastUIHeight);
            if (auto* pitchengaEditor = dynamic_cast<PitchengaAudioProcessorEditor*>(editor)) {
                pitchengaEditor->updateVisibilityFromState();
            }
        }

        // Restore the external plugin from the merged settings
        if (settings.externalPluginDescriptionXml.isNotEmpty()) {
            if (auto pluginDescriptionXml = juce::XmlDocument::parse(settings.externalPluginDescriptionXml)) {
                juce::PluginDescription pluginDescription;
                if (pluginDescription.loadFromXml(*pluginDescriptionXml)) {
                    // Safe UI cleanup before replacing the plugin
                    if (onPluginAboutToBeDeleted) {
                        onPluginAboutToBeDeleted();
                    }

                    loadExternalPlugin(pluginDescription);
                    
                    if (externalPlugin != nullptr && settings.externalPluginStateBase64.isNotEmpty()) {
                        juce::MemoryBlock pluginState;
                        if (pluginState.fromBase64Encoding(settings.externalPluginStateBase64)) {
                            externalPlugin->setStateInformation(pluginState.getData(), static_cast<int>(pluginState.getSize()));
                        }
                    }
                }
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
