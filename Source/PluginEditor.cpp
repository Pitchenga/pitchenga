#include "PluginProcessor.h"
#include "PluginEditor.h"

PitchengaAudioProcessorEditor::PitchengaAudioProcessorEditor(PitchengaAudioProcessor& p)
    : AudioProcessorEditor(&p),
    processor(p),
    worker(p),
    rollStft(p),
    rollCqt(p),
    splitter(p),
    control(p) {
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
    setSize(processor.settings.lastUIWidth, processor.settings.lastUIHeight);

    worker.startThread(juce::Thread::Priority::high);
    startTimerHz(uiRefreshRateHz);
}

PitchengaAudioProcessorEditor::~PitchengaAudioProcessorEditor() {
    worker.stopThread(2000);
    stopTimer();
}

void PitchengaAudioProcessorEditor::updateVisibilityFromState() {
    control.updateVisibilityFromState();
    resized();
}

void PitchengaAudioProcessorEditor::timerCallback() {
    // --- Update the Needle ---
    // Read the lock-free atomic variable
    const float latestPitchHz = processor.currentPitchHz.load(std::memory_order_relaxed);

    // Feed it to the needle visualizer component
    needle.setPitchFrequency(latestPitchHz);

    // --- Update the visualizers ---
    if (worker.hasNewData()) {
        worker.getEyeResults(eyeBuffer);
        worker.getRollStftResults(rollStftBuffer);
        worker.getRollCqtResults(rollCqtBuffer);
        worker.clearNewDataFlag();

        if (processor.settings.isShowEye) eye.updateResults(eyeBuffer);
        if (processor.settings.isShowRoll) {
            if (processor.settings.isUseRollStft) {
                rollStft.updateResults(rollStftBuffer);
            } else {
                rollCqt.updateResults(rollCqtBuffer);
            }
        }
    }

    //fixme: Is it needed?
    // Force continuous UI repaints to allow smooth sub-pixel interpolation of the scrolling visual
    // if (processor.settings.isShowRoll) {
    //     if (processor.settings.isUseRollStft) {
    //         rollStft.repaint();
    //     } else {
    //         rollCqt.repaint();
    //     }
    // }
}

void PitchengaAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);
}

void PitchengaAudioProcessorEditor::resized() {
    processor.settings.lastUIWidth = getWidth();
    processor.settings.lastUIHeight = getHeight();

    auto bounds = getLocalBounds();

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
