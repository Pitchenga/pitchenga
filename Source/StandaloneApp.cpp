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
    public juce::ChangeListener {
    juce::String preferredOutput;
    juce::String preferredInput;
    bool isRestoring = false;
    bool wasOnFallback = false;

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
                    wasOnFallback = false; // Mark immediately to prevent multi-triggering from UI panic

                    SafePointer safeThis(this);

                    // Fire asynchronously to bypass the UI lock from the Audio Settings window
                    juce::MessageManager::callAsync(
                        [safeThis, preferredOut = preferredOutput, preferredIn = preferredInput] {
                            if (safeThis == nullptr || safeThis->pluginHolder == nullptr) return;

                            const juce::ScopedValueSetter setter(safeThis->isRestoring, true);

                            juce::AudioDeviceManager::AudioDeviceSetup newSetup =
                                safeThis->pluginHolder->deviceManager.getAudioDeviceSetup();
                            newSetup.outputDeviceName = preferredOut;
                            newSetup.inputDeviceName = preferredIn;

                            auto error = safeThis->pluginHolder->deviceManager.setAudioDeviceSetup(newSetup, true);
                            if (error.isEmpty()) {
                                Util::log("Auto-restored preferred interface=" + preferredOut);
                            } else {
                                Util::log("Failed to auto-restore interface=" + error);
                            }
                        }
                    );
                } else {
                    // Manual Change: Preferred was plugged in, but user deliberately picked something else.
                    preferredOutput = currentSetup.outputDeviceName;
                    preferredInput = currentSetup.inputDeviceName;
                    Util::log("Updated preferred interface=" + preferredOutput);
                }
            } else {
                // Fallback Mode: Preferred is physically gone.
                wasOnFallback = true;
                Util::log(
                    "Preferred interface gone, switched to fallback interface="
                    + currentSetup.outputDeviceName
                );
            }
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchengaStandaloneWindow)
};

class [[maybe_unused]] PitchengaStandaloneApp : public juce::JUCEApplication {
public:
    PitchengaStandaloneApp() = default;

    const juce::String getApplicationName() override { return JucePlugin_Name; }
    const juce::String getApplicationVersion() override { return JucePlugin_VersionString; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String&) override {
        juce::PropertiesFile::Options options;
        options.applicationName = JucePlugin_Name;
        options.filenameSuffix = ".settings";
        // The following properties are ignored because we pass a specific File,
        // but they are required to cleanly initialize the Options object.
        options.osxLibrarySubFolder = "";
        options.folderName = "";

        // Force the settings file to sit alongside your presets using your sandboxed POSIX bypass
        juce::File settingsFile = Util::getAppFolder().getChildFile(juce::String(JucePlugin_Name) + ".settings");
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
