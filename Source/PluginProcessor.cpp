#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Util.h"
#include "BinaryData.h"
#include <stdexcept>

PitchengaAudioProcessor::PitchengaAudioProcessor()
    : AudioProcessor(
        BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    ) {
    Util::init();
    juce::addDefaultFormatsToManager(formatManager);

    // Load plugin list
    const auto appDataDir = Util::getApplicationDirectory();
    const auto pluginListFile = appDataDir.getChildFile("plugins.xml");
    if (pluginListFile.existsAsFile()) {
        if (auto xml = juce::XmlDocument::parse(pluginListFile)) {
            knownPluginList.recreateFromXml(*xml);
        }
    }

    loadDefaultSettings();
}

PitchengaAudioProcessor::~PitchengaAudioProcessor() {
    auto* plugin = atomicPlugin.exchange(nullptr);
    unloadPluginInstance(plugin);

    // Save plugin list
    const auto appDataDir = Util::getApplicationDirectory();
    const auto pluginListFile = appDataDir.getChildFile("plugins.xml");
    if (auto xml = knownPluginList.createXml()) {
        xml->writeTo(pluginListFile);
    }
}

void PitchengaAudioProcessor::loadDefaultSettings() {
    // Attempt to load user-default.xml from the "presets" sub-folder
    const auto appDataDir = Util::getApplicationDirectory();
    const auto presetsDir = appDataDir.getChildFile(Control::presetsDirectoryName);
    const auto userDefaultFile = presetsDir.getChildFile(Control::userDefaultPresetFileName);

    if (userDefaultFile.existsAsFile()) {
        if (auto xml = juce::XmlDocument::parse(userDefaultFile)) {
            // Tag name check is done in loadFromXml (which handles build profiles)
            if (settings.loadFromXml(*xml)) {
                return;
            }
        }
    }

    // Fallback to the factory setting bundled as binary data
    if (auto xml = juce::XmlDocument::parse(juce::String::createStringFromData(BinaryData::factorysettings_xml, BinaryData::factorysettings_xmlSize))) {
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

    if (auto* plugin = atomicPlugin.load()) {
        plugin->prepareToPlay(sampleRate, samplesPerBlock);
    }

    if (settings.isCaptureEnabled && wrapperType == wrapperType_Standalone) {
        desktopAudioCapture.start(sampleRate);
    }
    
    desktopAudioBuffer.assign(bufferSize, 0.0f);
}

void PitchengaAudioProcessor::releaseResources() {
    const juce::ScopedLock lock(pluginLock);
    if (auto* plugin = atomicPlugin.load()) {
        plugin->releaseResources();
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
    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    if (numSamples <= 0) return;

    // Safety check: if host exceeds promised block size, we must skip to avoid OOB/Allocation
    if (numSamples > micBuffer.getNumSamples() || numSamples > pluginOutputBuffer.getNumSamples()) return;

    // --- Audio Routing Matrix ---
    auto* plugin = atomicPlugin.load(std::memory_order_acquire);

    // Stream A: Microphone Intake
    // Use jmin to safely handle cases where the host buffer has fewer channels than expected
    const int numMicChannels = std::min(totalNumInputChannels, buffer.getNumChannels());
    for (int ch = 0; ch < numMicChannels; ++ch) {
        micBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);
    }

    // Stream B: Instrument Processing
    pluginOutputBuffer.clear(0, numSamples);

    if (plugin != nullptr) {
        plugin->processBlock(pluginOutputBuffer, midiMessages);
    }

    // Stream C: The Visualizer Feed (Blended Mic + Instrument)
    // We use monoBuffer as the destination for visualizers
    float* monoData = monoBuffer.data();
    std::fill(monoBuffer.begin(), monoBuffer.end(), 0.0f);

    // Mix Mic into monoData
    if (numMicChannels == 1) {
        juce::FloatVectorOperations::copy(monoData, micBuffer.getReadPointer(0), numSamples);
    } else if (numMicChannels >= 2) {
        juce::FloatVectorOperations::copy(monoData, micBuffer.getReadPointer(0), numSamples);
        juce::FloatVectorOperations::add(monoData, micBuffer.getReadPointer(1), numSamples);
        juce::FloatVectorOperations::multiply(monoData, 0.5f, numSamples);
    }

    // Mix Instrument into monoData (additive)
    const int numInstOutputChannels = pluginOutputBuffer.getNumChannels();
    if (numInstOutputChannels == 1) {
        juce::FloatVectorOperations::add(monoData, pluginOutputBuffer.getReadPointer(0), numSamples);
    } else if (numInstOutputChannels >= 2) {
        // Average the stereo output for the mono visualizer
        juce::FloatVectorOperations::add(monoData, pluginOutputBuffer.getReadPointer(0), numSamples);
        juce::FloatVectorOperations::add(monoData, pluginOutputBuffer.getReadPointer(1), numSamples);
        juce::FloatVectorOperations::multiply(monoData, 0.5f, numSamples);
    }

    // Stream D: Desktop Audio Capture
    int available = desktopAudioCapture.getFifo().getNumReady();

    // Catch up if we are falling behind to avoid latency. 
    // We use a large threshold (16384 samples ~370ms) to allow for jitter.
    if (available > 16384) {
        const int toSkip = available - numSamples;
        desktopAudioCapture.getFifo().finishedRead(toSkip);
        available = numSamples;
    }

    if (available > 0) {
        const int samplesToRead = std::min(available, numSamples);
        const auto scope = desktopAudioCapture.getFifo().read(samplesToRead);

        if (scope.blockSize1 > 0) {
            juce::FloatVectorOperations::add(monoData, desktopAudioCapture.getBuffer().data() + scope.startIndex1, scope.blockSize1);
        }

        if (scope.blockSize2 > 0) {
            juce::FloatVectorOperations::add(monoData + scope.blockSize1, desktopAudioCapture.getBuffer().data() + scope.startIndex2, scope.blockSize2);
        }
    }

    // Stream E: The Speaker Output
    buffer.clear();

    // Instrument always goes to speakers
    if (numInstOutputChannels > 0) {
        int sourceChannel = 0;
        for (int ch = 0; ch < totalNumOutputChannels; ++ch) {
            buffer.addFrom(ch, 0, pluginOutputBuffer, sourceChannel, 0, numSamples);
            if (++sourceChannel >= numInstOutputChannels) sourceChannel = 0;
        }
    }

    // Conditional input monitoring
    const int numMicInputChannels = micBuffer.getNumChannels();
    if (numMicInputChannels > 0) {
        if (wrapperType == wrapperType_Standalone) {
            if (settings.earVolumeLeft > 0.0f && totalNumOutputChannels > 0) {
                buffer.addFrom(0, 0, micBuffer, 0, 0, numSamples, settings.earVolumeLeft);
            }
            if (settings.earVolumeRight > 0.0f && totalNumOutputChannels > 1) {
                const int sourceChannel = 1 >= numMicInputChannels ? 0 : 1;
                buffer.addFrom(1, 0, micBuffer, sourceChannel, 0, numSamples, settings.earVolumeRight);
            }
        } else {
            int sourceChannel = 0;
            for (int channel = 0; channel < totalNumOutputChannels; ++channel) {
                buffer.addFrom(channel, 0, micBuffer, sourceChannel, 0, numSamples, 1.0f);
                if (++sourceChannel >= numMicInputChannels) sourceChannel = 0;
            }
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

void PitchengaAudioProcessor::loadExternalPlugin(const juce::PluginDescription& description, bool forceOpenWindow) {
    if (wrapperType != wrapperType_Standalone) return;

    // Control suspension externally to allow atomic operations like setup restoration
    // to keep processing suspended while state is restored.
    const bool wasSuspended = isSuspended();
    if (!wasSuspended) suspendProcessing(true);

    const double sampleRate = getSampleRate();
    const int blockSize = getBlockSize();

    Util::debug("Attempting to load plugin: " + description.name + " (" + description.fileOrIdentifier + ")");

    if (onPluginAboutToBeDeleted) {
        onPluginAboutToBeDeleted();
    }

    {
        const juce::ScopedLock lock(pluginLock);
        auto* oldInstance = atomicPlugin.exchange(nullptr, std::memory_order_release);
        unloadPluginInstance(oldInstance);
    }

    // Load the new plugin instance
    juce::String error;
    auto instance = formatManager.createPluginInstance(
        description,
        sampleRate > 0 ? sampleRate : 44100.0,
        blockSize > 0 ? blockSize : 512,
        error
    );

    if (instance != nullptr) {
        Util::debug("Successfully loaded plugin: " + instance->getName());
        if (forceOpenWindow) {
            settings.isExternalPluginWindowOpen = true;
        }

        {
            const juce::ScopedLock lock(pluginLock);

            // Ensure the new instance is prepared if we have valid host settings
            if (sampleRate > 0 && blockSize > 0) {
                instance->prepareToPlay(sampleRate, blockSize);
            }

            pluginOutputBuffer.setSize(
                instance->getTotalNumOutputChannels(),
                blockSize > 0 ? blockSize : 512,
                false,
                true,
                true
            );

            // Lock-Free Swap: Update the pointer used by the audio thread
            atomicPlugin.store(instance.release(), std::memory_order_release);
        }

        if (!wasSuspended) suspendProcessing(false);

        if (onPluginLoaded) {
            juce::MessageManager::callAsync([this] { if (onPluginLoaded) onPluginLoaded(); });
        }
    } else {
        if (!wasSuspended) suspendProcessing(false);
        Util::debug("Failed loading plugin, name=" + description.name + ", error=" + error);

        juce::MessageManager::callAsync([name = description.name, error] {
            juce::String detailedError = "Failed to load plugin '" + name + "':\n\n" + error;

            juce::AlertWindow::showMessageBoxAsync(
                juce::MessageBoxIconType::WarningIcon,
                "Plugin Load Error",
                detailedError
            );
        });
    }
}

void PitchengaAudioProcessor::unloadExternalPlugin() {
    suspendProcessing(true);
    if (onPluginAboutToBeDeleted) {
        onPluginAboutToBeDeleted();
    }
    {
        const juce::ScopedLock lock(pluginLock);
        auto* oldInstance = atomicPlugin.exchange(nullptr, std::memory_order_release);
        unloadPluginInstance(oldInstance);
    }
    suspendProcessing(false);
    if (onPluginLoaded) {
        juce::MessageManager::callAsync([this] { if (onPluginLoaded) onPluginLoaded(); });
    }
}

void PitchengaAudioProcessor::unloadPluginInstance(juce::AudioPluginInstance* instance) {
    if (instance == nullptr) return;

    // Explicitly release resources and delete the instance.
    // Since we are on the Message Thread and processing was suspended/resumed 
    // around the swap, it's safe to delete.
    instance->releaseResources();
    delete instance;
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

void PitchengaAudioProcessor::startDesktopCapture() {
    if (wrapperType == wrapperType_Standalone) {
        desktopAudioCapture.start(getSampleRate());
    }
}

void PitchengaAudioProcessor::stopDesktopCapture() {
    desktopAudioCapture.stop();
}

juce::AudioProcessorEditor* PitchengaAudioProcessor::createExternalPluginEditor() {
    const juce::ScopedLock lock(pluginLock);
    if (auto* plugin = atomicPlugin.load()) {
        return plugin->createEditorIfNeeded();
    }
    return nullptr;
}

void PitchengaAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    const juce::ScopedLock lock(pluginLock);

    // Sync UI state from editor if it exists
    if (auto* editor = getActiveEditor()) {
        settings.lastUiWidth = editor->getWidth();
        settings.lastUiHeight = editor->getHeight();

        if (auto* pitchengaEditor = dynamic_cast<PitchengaAudioProcessorEditor*>(editor)) {
            settings.isExternalPluginWindowOpen = pitchengaEditor->isPluginWindowOpen();
        }
    }

    settings.externalPluginDescriptionXml = {};
    settings.externalPluginStateBase64 = {};

    if (auto* plugin = atomicPlugin.load()) {
        juce::MemoryBlock pluginState;
        plugin->getStateInformation(pluginState);
        settings.externalPluginStateBase64 = pluginState.toBase64Encoding();

        auto pluginDescription = plugin->getPluginDescription();
        if (auto pluginDescriptionXml = pluginDescription.createXml()) {
            settings.externalPluginDescriptionXml = pluginDescriptionXml->toString();
        }
    }

    juce::XmlElement xml = settings.createXml();
    copyXmlToBinary(xml, destData);
}

void PitchengaAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    try {
        const std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

        if (xmlState == nullptr || !settings.loadFromXml(*xmlState)) {
            throw std::runtime_error(settingsErrorMessage.toStdString());
        }

        if (auto* editor = getActiveEditor()) {
            // Apply loaded size before triggering visibility updates which might cause nested resizes
            editor->setSize(settings.lastUiWidth, settings.lastUiHeight);
            if (auto* pitchengaEditor = dynamic_cast<PitchengaAudioProcessorEditor*>(editor)) {
                pitchengaEditor->updateVisibilityFromState();
            }
        }

        // Restore the external plugin from the merged settings
        if (settings.externalPluginDescriptionXml.isNotEmpty()) {
            auto pluginDescriptionXml = juce::XmlDocument::parse(settings.externalPluginDescriptionXml);
            if (pluginDescriptionXml == nullptr) {
                throw std::runtime_error(pluginRestorationErrorMessage.toStdString());
            }

            juce::PluginDescription pluginDescription;
            if (!pluginDescription.loadFromXml(*pluginDescriptionXml)) {
                throw std::runtime_error(pluginRestorationErrorMessage.toStdString());
            }

            // For setup restoration, we suspend processing during the entire operation
            // (including state setting) to avoid crashes like 0x10 (null dereference 
            // during processing while plugin is inconsistent).
            suspendProcessing(true);

            loadExternalPlugin(pluginDescription, false);

            if (auto* plugin = atomicPlugin.load()) {
                if (settings.externalPluginStateBase64.isNotEmpty()) {
                    juce::MemoryBlock pluginState;
                    if (pluginState.fromBase64Encoding(settings.externalPluginStateBase64)) {
                        const juce::ScopedLock lock(pluginLock);
                        plugin->setStateInformation(pluginState.getData(), static_cast<int>(pluginState.getSize()));
                    }
                }
            }

            suspendProcessing(false);

            // Restore the plugin window visibility state
            if (settings.isExternalPluginWindowOpen) {
                if (onShowExternalPluginEditor) {
                    onShowExternalPluginEditor();
                }
            }
        } else {
            unloadExternalPlugin();
        }
    } catch (const std::exception& e) {
        suspendProcessing(false);
        juce::MessageManager::callAsync(
            [message = juce::String(e.what())] {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::MessageBoxIconType::WarningIcon,
                    settingsErrorTitle,
                    message
                );
            }
        );
    }
}

juce::AudioProcessorEditor* PitchengaAudioProcessor::createEditor() {
    return new PitchengaAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new PitchengaAudioProcessor();
}



