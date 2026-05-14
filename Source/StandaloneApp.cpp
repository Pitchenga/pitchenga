#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>

#include <memory>

#include "Util.h"

#if JucePlugin_Build_Standalone

class PitchengaStandaloneWindow : public juce::StandaloneFilterWindow,
    public juce::ChangeListener,
    juce::Timer {
    juce::String preferredOutput;
    juce::String preferredInput;
    bool isRestoring = false;
    bool isOnFallbackOutput = false;
    bool isOnFallbackInput = false;
    bool isAttemptingRestoreOutput = false;
    bool isAttemptingRestoreInput = false;
    juce::String noneDevice = "<< none >>";

    void updateWindowTitle() {
        if (pluginHolder == nullptr) return;
        auto setup = pluginHolder->deviceManager.getAudioDeviceSetup();
        auto* type = pluginHolder->deviceManager.getCurrentDeviceTypeObject();

        juce::String outputName = setup.outputDeviceName;
        juce::String inputName = setup.inputDeviceName;

        // Sanitize against reality to ensure the UI reflects truth, not ghost strings
        if (type != nullptr) {
            if (!type->getDeviceNames(false).contains(outputName)) outputName = noneDevice;
            if (!type->getDeviceNames(true).contains(inputName)) inputName = noneDevice;
        }

        if (outputName.isEmpty()) outputName = noneDevice;
        if (inputName.isEmpty()) inputName = noneDevice;

        juce::String windowTitle = JucePlugin_Name;
        if (inputName == outputName) {
            windowTitle += " [" + inputName + "]";
        } else {
            windowTitle += " [" + inputName + "] [" + outputName + "]";
        }

        this->setName(windowTitle);
    }

    void timerCallback() override {
        stopTimer();
        if (pluginHolder == nullptr) return;

        juce::String error;
        {
            const juce::ScopedValueSetter setter(isRestoring, true);

            juce::AudioDeviceManager::AudioDeviceSetup newSetup =
                pluginHolder->deviceManager.getAudioDeviceSetup();

            // Wipe the inherited sample rate and buffer size so the hardware can auto-negotiate
            newSetup.sampleRate = 0.0;
            newSetup.bufferSize = 0;

            if (isAttemptingRestoreOutput) {
                newSetup.outputDeviceName = preferredOutput;
                newSetup.useDefaultOutputChannels = true;
                isOnFallbackOutput = false;
            } else if (isOnFallbackOutput && newSetup.outputDeviceName == preferredOutput) {
                newSetup.outputDeviceName = ""; // Wipe ghost fallback strings
            }

            if (isAttemptingRestoreInput) {
                newSetup.inputDeviceName = preferredInput;
                newSetup.useDefaultInputChannels = true;
                isOnFallbackInput = false;
            } else if (isOnFallbackInput && newSetup.inputDeviceName == preferredInput) {
                newSetup.inputDeviceName = ""; // Wipe ghost fallback strings
            }

            isAttemptingRestoreOutput = false;
            isAttemptingRestoreInput = false;

            // Force a complete teardown of the audio graph to revive MIDI and Desktop Capture
            pluginHolder->deviceManager.closeAudioDevice();

            error = pluginHolder->deviceManager.setAudioDeviceSetup(newSetup, true);
        } // isRestoring drops to false here, before we manually update the title

        if (error.isEmpty()) {
            Util::log("Hardware graph cleanly restarted.");
        } else {
            Util::log("Hardware graph restart failed=" + error);
        }

        // Explicitly force the title to update now that the restore lock is lifted
        updateWindowTitle();
    }

public:
    PitchengaStandaloneWindow(
        const juce::String& name,
        juce::Colour backgroundColour,
        std::unique_ptr<juce::StandalonePluginHolder> holder
    )
        : StandaloneFilterWindow(name, backgroundColour, std::move(holder)) {
        if (pluginHolder != nullptr) {
            if (pluginHolder->settings != nullptr) {
                // Read the preferred device from JUCE's native XML property storage
                if (std::unique_ptr savedState{
                    pluginHolder->settings->getXmlValue("PitchengaAudioSetup")
                }) {
                    preferredOutput = savedState->getStringAttribute("preferredOutput");
                    preferredInput = savedState->getStringAttribute("preferredInput");
                }
            }
            pluginHolder->deviceManager.addChangeListener(this);
        }

        updateWindowTitle();
    }

    ~PitchengaStandaloneWindow() override {
        if (pluginHolder != nullptr) {
            pluginHolder->deviceManager.removeChangeListener(this);

            if (pluginHolder->settings != nullptr) {
                // Save our preferred devices natively
                juce::XmlElement savedState("PitchengaAudioSetup");
                savedState.setAttribute("preferredOutput", preferredOutput);
                savedState.setAttribute("preferredInput", preferredInput);
                pluginHolder->settings->setValue("PitchengaAudioSetup", &savedState);
            }
        }
    }

    void changeListenerCallback(juce::ChangeBroadcaster* source) override {
        if (isRestoring || pluginHolder == nullptr) return;
        if (source != &pluginHolder->deviceManager) return;

        auto currentSetup = pluginHolder->deviceManager.getAudioDeviceSetup();
        auto* currentDeviceType = pluginHolder->deviceManager.getCurrentDeviceTypeObject();
        if (currentDeviceType == nullptr) return;

        auto availableOutputs = currentDeviceType->getDeviceNames(false);
        auto availableInputs = currentDeviceType->getDeviceNames(true);

        // Explicitly destroy JUCE ghost strings by validating against physical reality
        if (!availableOutputs.contains(currentSetup.outputDeviceName)) {
            currentSetup.outputDeviceName = "";
        }
        if (!availableInputs.contains(currentSetup.inputDeviceName)) {
            currentSetup.inputDeviceName = "";
        }

        // Initial setup: capture current as preferred if none exists
        if (preferredOutput.isEmpty() && currentSetup.outputDeviceName.isNotEmpty()) {
            preferredOutput = currentSetup.outputDeviceName;
            isOnFallbackOutput = false;
        }
        if (preferredInput.isEmpty() && currentSetup.inputDeviceName.isNotEmpty()) {
            preferredInput = currentSetup.inputDeviceName;
            isOnFallbackInput = false;
        }

        if (currentSetup.outputDeviceName == preferredOutput
            && currentSetup.inputDeviceName == preferredInput) {
            isOnFallbackOutput = false;
            isOnFallbackInput = false;
            updateWindowTitle();
            return;
        }

        bool triggerRestart = false;

        // --- Handle Output ---
        if (preferredOutput.isNotEmpty()) {
            if (!availableOutputs.contains(preferredOutput)) {
                // Fallback Mode: Preferred is physically gone.
                if (!isOnFallbackOutput) {
                    isOnFallbackOutput = true;
                    triggerRestart = true;
                    Util::log(
                        "Preferred output gone, switched to fallback output=" + currentSetup.outputDeviceName
                    );
                }
            } else if (isOnFallbackOutput) {
                // Auto-Restore: We were on a fallback, and preferred came back
                isAttemptingRestoreOutput = true;
                triggerRestart = true;
            } else if (currentSetup.outputDeviceName != preferredOutput) {
                // Manual Change: Preferred was plugged in, but user deliberately picked something else
                preferredOutput = currentSetup.outputDeviceName;
                Util::log("Updated preferred output=" + preferredOutput);
            }
        }

        // --- Handle Input ---
        if (preferredInput.isNotEmpty()) {
            if (!availableInputs.contains(preferredInput)) {
                // Fallback Mode: Preferred is physically gone.
                if (!isOnFallbackInput) {
                    isOnFallbackInput = true;
                    triggerRestart = true;
                    Util::log(
                        "Preferred input gone, switched to fallback input=" + currentSetup.inputDeviceName
                    );
                }
            } else if (isOnFallbackInput) {
                // Auto-Restore: We were on a fallback, and preferred came back
                isAttemptingRestoreInput = true;
                triggerRestart = true;
            } else if (currentSetup.inputDeviceName != preferredInput) {
                // Manual Change: Preferred was plugged in, but user deliberately picked something else
                preferredInput = currentSetup.inputDeviceName;
                Util::log("Updated preferred input=" + preferredInput);
            }
        }

        updateWindowTitle();

        if (triggerRestart) {
            // Debounce for 800ms to allow OS to settle the USB connection before tearing down the graph
            startTimer(800);
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchengaStandaloneWindow)
};

class [[maybe_unused]] PitchengaStandaloneApp : public juce::JUCEApplication {
public:
    PitchengaStandaloneApp() = default;
    const juce::String settingsSuffix = ".settings.xml";

    const juce::String getApplicationName() override { return JucePlugin_Name; }
    const juce::String getApplicationVersion() override { return JucePlugin_VersionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String&) override {
        // Run JUCE's default setup first (handles all the heavy lifting)
        juce::PropertiesFile::Options options;
        options.applicationName = JucePlugin_Name;
        options.filenameSuffix = settingsSuffix;
        options.osxLibrarySubFolder = "";
        options.folderName = "";

        const juce::File settingsFile = Util::getAppFolder()
            .getChildFile(juce::String(JucePlugin_Name) + settingsSuffix);
        auto* propertiesFile = new juce::PropertiesFile(settingsFile, options);

        auto* holder = new juce::StandalonePluginHolder(
            propertiesFile,
            true,
            // Tells the holder to take memory ownership of our custom propertiesFile
            {},
            nullptr
        );

        window = std::make_unique<PitchengaStandaloneWindow>(
            getApplicationName(),
            juce::LookAndFeel::getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
            std::unique_ptr<juce::StandalonePluginHolder>(holder)
        );

        window->setVisible(true);
    }

    void shutdown() override {
        window.reset();
    }

    void systemRequestedQuit() override {
        if (window != nullptr && window->pluginHolder != nullptr) {
            window->pluginHolder->savePluginState();
        }
        quit();
    }

private:
    std::unique_ptr<PitchengaStandaloneWindow> window;
};

// This macro tells JUCE to safely boot our class instead of the default internal one,
// avoiding duplicate main() linker errors.
// ReSharper disable once CppStaticAssertFailure
START_JUCE_APPLICATION(PitchengaStandaloneApp)

#endif
