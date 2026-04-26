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
    needle(p),
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
    control.onVisibilityChanged = [this] {
        rollStft.resized();
        rollCqt.resized();
        resized();
    };

    addAndMakeVisible(splitter);
    splitter.onDragged = [this] { resized(); };

    eyeBuffer.resize(Eye::totalFoldedBins, 0.0);
    rollCqtBuffer.resize(PitchengaAudioProcessor::numOctaves * 12);

    setResizable(true, true);
    setWantsKeyboardFocus(true);
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

    const bool isShowRoll = processor.settings.isShowRoll;
    const bool isShowEye = processor.settings.isShowEye;
    const bool isShowNeedle = processor.settings.isShowNeedle;
    const bool isLayoutHorizontal = processor.settings.isLayoutHorizontal;

    rollStft.setVisible(isShowRoll && processor.settings.isUseRollStft);
    rollCqt.setVisible(isShowRoll && !processor.settings.isUseRollStft);
    eye.setVisible(isShowEye);
    needle.setVisible(isShowNeedle);

    // Only show splitter if both resizable elements (Roll and Eye) are active
    splitter.setVisible(isShowRoll && isShowEye);

    const int needleHeight = isShowNeedle ? static_cast<int>(Needle::getPreferredHeight()) : 0;

    if (isShowRoll && isShowEye) {
        if (isLayoutHorizontal) {
            // --- Horizontal Layout: [Eye/Needle] |Split| [Roll] ---
            const int availableWidth = bounds.getWidth() - 4; // 4 is splitter width
            const int eyeWidth = static_cast<int>(static_cast<float>(availableWidth) * (1.0f - processor.settings.splitRatio));
            
            auto eyeNeedleRect = bounds.removeFromLeft(eyeWidth);
            if (isShowNeedle) {
                needle.setBounds(eyeNeedleRect.removeFromBottom(needleHeight));
                eyeNeedleRect.removeFromBottom(1); // 1px gap
            }
            eye.setBounds(eyeNeedleRect);

            splitter.setBounds(bounds.removeFromLeft(4));
            
            if (processor.settings.isUseRollStft) {
                rollStft.setBounds(bounds);
            } else {
                rollCqt.setBounds(bounds);
            }
        } else {
            // --- Vertical Layout: [Roll] / [Eye] / [Needle] ---
            const int availableHeight = bounds.getHeight() - needleHeight - 4; // 4 is splitter height
            const int rollHeight = static_cast<int>(static_cast<float>(availableHeight) * processor.settings.splitRatio);

            auto rollRect = bounds.removeFromTop(rollHeight);
            if (processor.settings.isUseRollStft) {
                rollStft.setBounds(rollRect);
            } else {
                rollCqt.setBounds(rollRect);
            }

            splitter.setBounds(bounds.removeFromTop(4));
            
            if (isShowNeedle) {
                needle.setBounds(bounds.removeFromBottom(needleHeight));
                bounds.removeFromBottom(1); // 1px gap
            }
            eye.setBounds(bounds);
        }
    } else {
        // --- Simplified stack logic when one primary element is missing ---
        if (isShowRoll) {
            auto rollRect = bounds;
            if (isShowNeedle) {
                needle.setBounds(rollRect.removeFromBottom(needleHeight));
            }
            if (processor.settings.isUseRollStft) {
                rollStft.setBounds(rollRect);
            } else {
                rollCqt.setBounds(rollRect);
            }
        } else if (isShowEye) {
            if (isShowNeedle) {
                needle.setBounds(bounds.removeFromBottom(needleHeight));
                bounds.removeFromBottom(1); // 1px gap
            }
            eye.setBounds(bounds);
        }
    }
}

bool PitchengaAudioProcessorEditor::keyPressed(const juce::KeyPress& key) {
    if (key == juce::KeyPress::returnKey || key == juce::KeyPress('f') || key == juce::KeyPress('F')) {
        if (auto* topLevel = getTopLevelComponent()) {
            if (auto* window = dynamic_cast<juce::ResizableWindow*>(topLevel)) {
                window->setFullScreen(!window->isFullScreen());
                return true;
            }
        }
    }
    return false;
}
