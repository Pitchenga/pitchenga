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
    processor.onPluginLoaded = [this] { control.updateVisibilityFromState(); };

    addAndMakeVisible(needle);
    addAndMakeVisible(eye);
    addAndMakeVisible(rollStft);
    addAndMakeVisible(rollCqt);

    addAndMakeVisible(control);
    control.onVisibilityChanged = [this] { resized(); };

    addAndMakeVisible(splitter);
    splitter.onDragged = [this] { resized(); };

    eyeBuffer.resize(Eye::totalFoldedBins, 0.0);

    setResizable(true, true);
    setResizeLimits(400, 600, 2000, 2000);
    setSize(processor.settings.lastUIWidth, processor.settings.lastUIHeight);

    startTimerHz(uiRefreshRateHz);
    worker.startThread(juce::Thread::Priority::high);
}

PitchengaAudioProcessorEditor::~PitchengaAudioProcessorEditor() {
    stopTimer();
    worker.stopThread(2000);

    // Clear callbacks to avoid dangling pointers
    processor.onShowExternalPluginEditor = nullptr;
    processor.onOpenPluginBrowser = nullptr;
    processor.onPluginLoaded = nullptr;
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
    if (pluginWindow == nullptr) {
        pluginWindow = std::make_unique<ExternalPluginWindow>(
            "Instrument",
            juce::Colours::black,
            juce::DocumentWindow::closeButton
        );
        pluginWindow->setUsingNativeTitleBar(true);
    }
    
    if (auto* externalEditor = processor.createExternalPluginEditor()) {
        pluginWindow->setContentNonOwned(externalEditor, true);
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
        browserWindow->setSize(600, 400);
        
        auto& formatManager = processor.getFormatManager();
        auto& knownPluginList = processor.getKnownPluginList();
        
        auto listComponent = std::make_unique<juce::PluginListComponent>(
            formatManager, 
            knownPluginList, 
            juce::File(), 
            nullptr
        );
        
        browserWindow->setContentOwned(listComponent.release(), true);
    }
    
    browserWindow->setVisible(true);
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
    processor.settings.lastUIWidth = getWidth();
    processor.settings.lastUIHeight = getHeight();

    // Give the control bar its own dedicated, non-overlapping space at the top left
    control.setBounds(bounds.removeFromTop(static_cast<int>(control.getPreferredHeight())));

    rollStft.setVisible(processor.settings.isShowRoll && processor.settings.isUseRollStft);
    rollCqt.setVisible(processor.settings.isShowRoll && !processor.settings.isUseRollStft);
    eye.setVisible(processor.settings.isShowEye);
    needle.setVisible(processor.settings.isShowNeedle);

    // Only show splitter if both resizable elements are active
    splitter.setVisible(processor.settings.isShowRoll && processor.settings.isShowEye);

    const int needleHeight = processor.settings.isShowNeedle ? static_cast<int>(Needle::getPreferredHeight() + 1) : 0;

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
