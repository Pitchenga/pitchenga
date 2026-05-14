#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>
#include "PluginProcessor.h"
#include "Util.h"

#if JucePlugin_Build_Standalone

namespace juce {
    class PitchengaStandaloneWindow : public StandaloneFilterWindow,
        public ChangeListener {
    public:
        PitchengaStandaloneWindow(
            const String& name,
            Colour backgroundColour,
            std::unique_ptr<StandalonePluginHolder> holder
        )
            : StandaloneFilterWindow(name, backgroundColour, std::move(holder)) {
            getDeviceManager().addChangeListener(this);

            if (auto* processor = dynamic_cast<PitchengaAudioProcessor*>(getAudioProcessor())) {
                processor->onShowAudioSettings = [this] {
                    showAudioSettingsDialog();
                };
            }
        }

        ~PitchengaStandaloneWindow() override {
            getDeviceManager().removeChangeListener(this);
        }

        void changeListenerCallback(ChangeBroadcaster* source) override {
            if (source == &getDeviceManager()) {
                if (isRestoring) return;

                auto* processor = dynamic_cast<PitchengaAudioProcessor*>(getAudioProcessor());
                if (processor == nullptr) return;

                auto& settings = processor->settings;
                auto currentSetup = getDeviceManager().getAudioDeviceSetup();

                // 1. Initial setup: capture current as preferred if none exists
                if (settings.preferredOutputDevice.isEmpty() && currentSetup.outputDeviceName.isNotEmpty()) {
                    settings.preferredOutputDevice = currentSetup.outputDeviceName;
                    settings.preferredInputDevice = currentSetup.inputDeviceName;
                    wasOnFallback = false;
                    return;
                }

                if (currentSetup.outputDeviceName == settings.preferredOutputDevice) {
                    wasOnFallback = false;
                    return;
                }

                // 2. We are not on the preferred device.
                if (auto* currentDeviceType = getDeviceManager().getCurrentDeviceTypeObject()) {
                    auto availableDevices = currentDeviceType->getDeviceNames();

                    if (availableDevices.contains(settings.preferredOutputDevice)) {
                        // Preferred is available, but we are not on it.

                        if (wasOnFallback) {
                            // 3. Auto-Restore: We were on a fallback, and preferred came back.
                            const ScopedValueSetter<bool> setter(isRestoring, true);

                            AudioDeviceManager::AudioDeviceSetup newSetup = currentSetup;
                            newSetup.outputDeviceName = settings.preferredOutputDevice;
                            newSetup.inputDeviceName = settings.preferredInputDevice;

                            getDeviceManager().setAudioDeviceSetup(newSetup, true);
                            Util::log("Auto-restored preferred interface: " + settings.preferredOutputDevice);
                            wasOnFallback = false;
                        } else {
                            // 4. Manual Change: Preferred was available, but user picked something else.
                            settings.preferredOutputDevice = currentSetup.outputDeviceName;
                            settings.preferredInputDevice = currentSetup.inputDeviceName;
                            Util::log("Updated preferred interface to: " + settings.preferredOutputDevice);
                        }
                    } else {
                        // 5. Fallback Mode: Preferred is gone.
                        wasOnFallback = true;
                        Util::log("Preferred interface gone, entering fallback mode: " + currentSetup.outputDeviceName);
                    }
                }
            }
        }

        // We override this to capture when the user deliberately changes settings
        void showAudioSettingsDialog() {
            if (pluginHolder != nullptr) pluginHolder->showAudioSettingsDialog();
        }

    private:
        bool isRestoring = false;
        bool wasOnFallback = false;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchengaStandaloneWindow)
    };

    class PitchengaStandaloneApp : public JUCEApplication {
    public:
        PitchengaStandaloneApp() {
            PropertiesFile::Options options;
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

        const String getApplicationName() override { return JucePlugin_Name; }
        const String getApplicationVersion() override { return JucePlugin_VersionString; }
        bool moreThanOneInstanceAllowed() override { return true; }

        void initialise(const String&) override {
            window.reset(createWindow());
        }

        void shutdown() override {
            window.reset();
        }

        PitchengaStandaloneWindow* createWindow() {
            auto* holder = new StandalonePluginHolder(
                appProperties.getUserSettings(),
                false,
                {},
                nullptr
            );

            return new PitchengaStandaloneWindow(
                getApplicationName(),
                LookAndFeel::getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId),
                std::unique_ptr<StandalonePluginHolder>(holder)
            );
        }

        void systemRequestedQuit() override {
            if (window != nullptr) window->pluginHolder->savePluginState();

            quit();
        }

    private:
        ApplicationProperties appProperties;
        std::unique_ptr<PitchengaStandaloneWindow> window;
    };
} // namespace juce

START_JUCE_APPLICATION(juce::PitchengaStandaloneApp)

#endif
