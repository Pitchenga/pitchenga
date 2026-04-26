#include "PluginProcessor.h"
#include "PluginEditor.h"

// --- Custom Plugin Window ---
class ExternalPluginWindow : public juce::DocumentWindow {
public:
    ExternalPluginWindow(const juce::String& name, juce::Colour backgroundColour, int buttons)
        : DocumentWindow(name, backgroundColour, buttons) {}

    void closeButtonPressed() override {
        setVisible(false);
    }
};

PitchengaAudioProcessorEditor::PitchengaAudioProcessorEditor(PitchengaAudioProcessor& p)
    : AudioProcessorEditor(&p),
    processor(p),
    worker(p),
    rollStft(p),
    rollCqt(p),
    splitter(p),
    control(p) {

    // Set processor callbacks
    processor.onShowExternalPluginEditor = [this] { openPluginWindow(); };
    processor.onOpenPluginBrowser = [this] { openPluginBrowserWindow(); };
    processor.onRescanPlugins = [this] { startPluginScan(); };
    processor.onPluginLoaded = [this] {
        control.updateVisibilityFromState();
        if (processor.settings.isExternalPluginWindowOpen) {
            openPluginWindow();
        }
    };
    processor.onPluginAboutToBeDeleted = [this] { pluginWindow = nullptr; };

    addAndMakeVisible(needle);
    addAndMakeVisible(eye);
    addAndMakeVisible(rollStft);
    addAndMakeVisible(rollCqt);

    addAndMakeVisible(control);
    control.onVisibilityChanged = [this] { resized(); };

    addAndMakeVisible(splitter);
    splitter.onDragged = [this] { resized(); };

    eyeBuffer.resize(Eye::totalFoldedBins, 0.0);
    rollCqtBuffer.resize(PitchengaAudioProcessor::numOctaves * 12);

    setResizable(true, true);
    setSize(processor.settings.lastUiWidth, processor.settings.lastUiHeight);

    startTimerHz(uiRefreshRateHz);
    worker.startThread(juce::Thread::Priority::high);
}

PitchengaAudioProcessorEditor::~PitchengaAudioProcessorEditor() {
    stopTimer();
    worker.stopThread(2000);

    // Clear callbacks to avoid dangling pointers
    processor.onShowExternalPluginEditor = nullptr;
    processor.onOpenPluginBrowser = nullptr;
    processor.onRescanPlugins = nullptr;
    processor.onPluginLoaded = nullptr;
    processor.onPluginAboutToBeDeleted = nullptr;
}

void PitchengaAudioProcessorEditor::timerCallback() {
    if (worker.hasNewData()) {
        worker.getEyeResults(eyeBuffer);
        worker.getRollStftResults(rollStftBuffer);
        worker.getRollCqtResults(rollCqtBuffer);
        
        eye.updateResults(eyeBuffer);
        rollStft.updateResults(rollStftBuffer);
        rollCqt.updateResults(rollCqtBuffer);
        
        worker.clearNewDataFlag();
    }
    
    // Smoothly poll current pitch for the needle
    const float latestPitchHz = processor.currentPitchHz.load(std::memory_order_relaxed);
    needle.setPitchFrequency(latestPitchHz);
}

void PitchengaAudioProcessorEditor::openPluginWindow() {
    // Destroy the existing window to ensure we start fresh with the new plugin's preferred size and title
    pluginWindow = nullptr;

    if (auto* externalEditor = processor.createExternalPluginEditor()) {
        pluginWindow = std::make_unique<ExternalPluginWindow>(
            externalEditor->getAudioProcessor()->getName(),
            juce::Colours::black,
            juce::DocumentWindow::closeButton
        );
        pluginWindow->setUsingNativeTitleBar(true);
        pluginWindow->setContentOwned(externalEditor, true);
        pluginWindow->setVisible(true);
    }
}

void PitchengaAudioProcessorEditor::openPluginBrowserWindow() {
    if (browserWindow == nullptr) {
        browserWindow = std::make_unique<ExternalPluginWindow>(
            "Plugin Browser",
            juce::Colours::darkgrey,
            juce::DocumentWindow::closeButton
        );
        browserWindow->setUsingNativeTitleBar(true);
        browserWindow->setResizable(true, true);
        browserWindow->setSize(900, 900);
        
        auto& formatManager = processor.getFormatManager();
        auto& knownPluginList = processor.getKnownPluginList();
        
        auto list = std::make_unique<juce::PluginListComponent>(
            formatManager, 
            knownPluginList, 
            juce::File(), 
            nullptr
        );
        
        listComponent = list.get();
        browserWindow->setContentOwned(list.release(), true);
    }
    
    browserWindow->setVisible(true);
}

void PitchengaAudioProcessorEditor::startPluginScan() {
    // Ensure the browser component is created so we can use its scanning functionality
    openPluginBrowserWindow();
    
    if (listComponent != nullptr) {
        auto& formatManager = processor.getFormatManager();
        for (int i = 0; i < formatManager.getNumFormats(); ++i) {
            if (auto* format = formatManager.getFormat(i)) {
                // This triggers the JUCE "Scanning plugins..." progress window
                listComponent->scanFor(*format);
            }
        }
    }
    
    // Close the residual plugin manager window after the scan is complete as requested
    if (browserWindow != nullptr) {
        browserWindow->setVisible(false);
    }
}

void PitchengaAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);
}

void PitchengaAudioProcessorEditor::updateVisibilityFromState() {
    control.updateVisibilityFromState();
    resized();
}

void PitchengaAudioProcessorEditor::resized() {
    auto bounds = getLocalBounds();

    // Give the control bar its own dedicated, non-overlapping space at the top left
    control.setBounds(bounds.removeFromTop(static_cast<int>(control.getPreferredHeight())));

    rollStft.setVisible(processor.settings.isShowRoll && processor.settings.isUseRollStft);
    rollCqt.setVisible(processor.settings.isShowRoll && !processor.settings.isUseRollStft);
    eye.setVisible(processor.settings.isShowEye);
    needle.setVisible(processor.settings.isShowNeedle);

    // Only show splitter if both resizable elements are active
    splitter.setVisible(processor.settings.isShowRoll && processor.settings.isShowEye);

    const int needleHeight = processor.settings.isShowNeedle ? static_cast<int>(Needle::getPreferredHeight()) : 0;

    if (processor.settings.isShowRoll && processor.settings.isShowEye) {
        const int availableHeight = bounds.getHeight() - needleHeight - 4; // 4 is splitter height
        const int rollHeight = static_cast<int>(static_cast<float>(availableHeight) * processor.settings.splitRatio);

        auto rollRect = bounds.removeFromTop(rollHeight);
        if (processor.settings.isUseRollStft) {
            rollStft.setBounds(rollRect);
        } else {
            rollCqt.setBounds(rollRect);
        }

        if (processor.settings.isShowNeedle) {
            needle.setBounds(bounds.removeFromTop(needleHeight));
        }

        splitter.setBounds(bounds.removeFromTop(4)); // Dedicated 4px hit-box for the resizer
        eye.setBounds(bounds);
    } else {
        // Vertical stack logic when eye or roll is missing (no splitter)
        if (processor.settings.isShowRoll) {
            const int rollHeight = bounds.getHeight() - needleHeight - (processor.settings.isShowEye ? bounds.getHeight() / 2 : 0);
            auto rollRect = bounds.removeFromTop(rollHeight);
            if (processor.settings.isUseRollStft) {
                rollStft.setBounds(rollRect);
            } else {
                rollCqt.setBounds(rollRect);
            }
        }

        if (processor.settings.isShowNeedle) {
            needle.setBounds(bounds.removeFromTop(needleHeight));
        }

        if (processor.settings.isShowEye) {
            eye.setBounds(bounds);
        }
    }
}
