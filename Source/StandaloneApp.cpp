#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>

#if JucePlugin_Build_Standalone

class PitchengaStandaloneWindow : public juce::StandaloneFilterWindow,
                                  public juce::ChangeListener {
private:
    juce::String preferredOutput;
    juce::String preferredInput;
    bool isRestoring = false;
    bool wasOnFallback = false;

public:
    PitchengaStandaloneWindow(const juce::String& name,
                              juce::Colour backgroundColour,
                              std::unique_ptr<juce::StandalonePluginHolder> holder)
        : juce::StandaloneFilterWindow(name, backgroundColour, std::move(holder))
    {
        if (pluginHolder != nullptr) {
            if (pluginHolder->settings != nullptr) {
                // Read the preferred device from JUCE's native XML property storage
                if (std::unique_ptr<juce::XmlElement> savedState { pluginHolder->settings->getXmlValue("PitchengaAudioSetup") }) {
                    preferredOutput = savedState->getStringAttribute("preferredOutput");
                    preferredInput = savedState->getStringAttribute("preferredInput");
                }
            }
            pluginHolder->deviceManager.addChangeListener(this);
        }
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
        if (source != &(pluginHolder->deviceManager)) return;

        auto currentSetup = pluginHolder->deviceManager.getAudioDeviceSetup();

        // Initial setup: capture current as preferred if none exists
        if (preferredOutput.isEmpty() && currentSetup.outputDeviceName.isNotEmpty()) {
            preferredOutput = currentSetup.outputDeviceName;
            preferredInput = currentSetup.inputDeviceName;
            wasOnFallback = false;
            return;
        }

        if (currentSetup.outputDeviceName == preferredOutput) {
            wasOnFallback = false;
            return;
        }

        // We are not on the preferred device.
        if (auto* currentDeviceType = pluginHolder->deviceManager.getCurrentDeviceTypeObject()) {
            auto availableDevices = currentDeviceType->getDeviceNames();

            if (availableDevices.contains(preferredOutput)) {
                // Preferred is available, but we are not on it.
                if (wasOnFallback) {
                    // Auto-Restore: We were on a fallback, and preferred came back!
                    const juce::ScopedValueSetter<bool> setter(isRestoring, true);

                    juce::AudioDeviceManager::AudioDeviceSetup newSetup = currentSetup;
                    newSetup.outputDeviceName = preferredOutput;
                    newSetup.inputDeviceName = preferredInput;

                    pluginHolder->deviceManager.setAudioDeviceSetup(newSetup, true);
                    juce::Logger::writeToLog("Auto-restored preferred interface: " + preferredOutput);
                    wasOnFallback = false;
                } else {
                    // Manual Change: Preferred was plugged in, but user deliberately picked something else.
                    preferredOutput = currentSetup.outputDeviceName;
                    preferredInput = currentSetup.inputDeviceName;
                    juce::Logger::writeToLog("Updated preferred interface to: " + preferredOutput);
                }
            } else {
                // Fallback Mode: Preferred is physically gone.
                wasOnFallback = true;
                juce::Logger::writeToLog("Preferred interface gone, entering fallback mode: " + currentSetup.outputDeviceName);
            }
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchengaStandaloneWindow)
};

class PitchengaStandaloneApp : public juce::JUCEApplication {
public:
    PitchengaStandaloneApp() {
        juce::PropertiesFile::Options options;
        options.applicationName = JucePlugin_Name;
        options.filenameSuffix = ".settings";
        options.osxLibrarySubFolder = "Application Support";
#if JUCE_LINUX || JUCE_BSD
        options.folderName = "~/.config";
#else
        options.folderName = "";
#endif
        appProperties.setStorageParameters(options);
    }

    const juce::String getApplicationName() override { return JucePlugin_Name; }
    const juce::String getApplicationVersion() override { return JucePlugin_VersionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String&) override {
        // Run JUCE's default setup first (handles all the heavy lifting)
        auto* holder = new juce::StandalonePluginHolder(
            appProperties.getUserSettings(),
            false,
            {},
            nullptr
        );

        window.reset(new PitchengaStandaloneWindow(
            getApplicationName(),
            juce::LookAndFeel::getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId),
            std::unique_ptr<juce::StandalonePluginHolder>(holder)
        ));

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
    juce::ApplicationProperties appProperties;
    std::unique_ptr<PitchengaStandaloneWindow> window;
};

// This macro tells JUCE to safely boot our class instead of the default internal one,
// avoiding duplicate main() linker errors.
START_JUCE_APPLICATION(PitchengaStandaloneApp)

#endif