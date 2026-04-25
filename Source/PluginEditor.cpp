#include "PluginProcessor.h"
#include "PluginEditor.h"

PitchengaAudioProcessorEditor::PitchengaAudioProcessorEditor(PitchengaAudioProcessor& p)
    : AudioProcessorEditor(&p),
    processor(p),
    worker(p),
    stftRoll(p),
    cqtRoll(p),
    splitter(p),
    control(p) {
    addAndMakeVisible(needle);
    addAndMakeVisible(eye);
    addAndMakeVisible(stftRoll);
    addAndMakeVisible(cqtRoll);

    addAndMakeVisible(control);
    control.onVisibilityChanged = [this] { resized(); };

    addAndMakeVisible(splitter);
    splitter.onDragged = [this] { resized(); };

    circleBuffer.resize(Eye::totalFoldedBins, 0.0);

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
        worker.getCircleResults(circleBuffer);
        worker.getRollPeaks(rollPeaksBuffer);
        worker.getLineResults(cqtRollBuffer);
        worker.clearNewDataFlag();

        if (processor.settings.isShowEye) eye.updateResults(circleBuffer);
        if (processor.settings.isShowRoll) {
            if (processor.settings.isUseStftRoll) {
                stftRoll.updateResults(rollPeaksBuffer);
            } else {
                cqtRoll.updateResults(cqtRollBuffer);
            }
        }
    }

    //fixme: Is it needed?
    // Force continuous UI repaints to allow smooth sub-pixel interpolation of the scrolling visual
    // if (processor.settings.isShowRoll) {
    //     if (processor.settings.isUseStftRoll) {
    //         stftRoll.repaint();
    //     } else {
    //         cqtRoll.repaint();
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

    stftRoll.setVisible(processor.settings.isShowRoll && processor.settings.isUseStftRoll);
    cqtRoll.setVisible(processor.settings.isShowRoll && !processor.settings.isUseStftRoll);
    eye.setVisible(processor.settings.isShowEye);
    needle.setVisible(processor.settings.isShowNeedle);

    // Only show splitter if both resizable elements are active
    splitter.setVisible(processor.settings.isShowRoll && processor.settings.isShowEye);

    const int needleHeight = processor.settings.isShowNeedle ? static_cast<int>(Needle::getPreferredHeight() + 1) : 0;

    if (processor.settings.isShowRoll && processor.settings.isShowEye) {
        const int availableHeight = bounds.getHeight() - needleHeight - 4; // 4 is splitter height
        const int rollHeight = static_cast<int>(static_cast<float>(availableHeight) * processor.settings.splitRatio);

        auto rollRect = bounds.removeFromTop(rollHeight);
        if (processor.settings.isUseStftRoll) {
            stftRoll.setBounds(rollRect);
        } else {
            cqtRoll.setBounds(rollRect);
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
            if (processor.settings.isUseStftRoll) {
                stftRoll.setBounds(rollRect);
            } else {
                cqtRoll.setBounds(rollRect);
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
