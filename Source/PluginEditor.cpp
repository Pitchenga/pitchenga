#include "PluginProcessor.h"
#include "PluginEditor.h"

PitchengaAudioProcessorEditor::PitchengaAudioProcessorEditor(PitchengaAudioProcessor& p)
    : AudioProcessorEditor(&p),
    processor(p),
    worker(p),
    needle(p),
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

        if (processor.settings.showEye) eye.updateResults(circleBuffer);
        if (processor.settings.showRoll) {
            if (processor.settings.useStftRoll) {
                stftRoll.updateResults(rollPeaksBuffer);
            } else {
                cqtRoll.updateResults(cqtRollBuffer);
            }
        }
    }

    //fixme: Is it needed?
    // Force continuous UI repaints to allow smooth sub-pixel interpolation of the scrolling visual
    // if (processor.settings.showRoll) {
    //     if (processor.settings.useStftRoll) {
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

    stftRoll.setVisible(processor.settings.showRoll && processor.settings.useStftRoll);
    cqtRoll.setVisible(processor.settings.showRoll && !processor.settings.useStftRoll);
    eye.setVisible(processor.settings.showEye);
    needle.setVisible(processor.settings.showNeedle);

    // Only show splitter if both resizable elements are active
    splitter.setVisible(processor.settings.showRoll && processor.settings.showEye);

    if (processor.settings.showNeedle) {
        needle.setBounds(bounds.removeFromBottom(static_cast<int>(Needle::getPreferredHeight() + 1)));
    }

    if (processor.settings.showRoll && processor.settings.showEye) {
        const int availableHeight = bounds.getHeight();
        const int rollHeight = static_cast<int>(static_cast<float>(availableHeight) * processor.settings.splitRatio);

        if (processor.settings.useStftRoll) {
            stftRoll.setBounds(bounds.removeFromTop(rollHeight));
        } else {
            cqtRoll.setBounds(bounds.removeFromTop(rollHeight));
        }
        splitter.setBounds(bounds.removeFromTop(4)); // Dedicated 4px hit-box for the resizer
        eye.setBounds(bounds);
    } else if (processor.settings.showRoll) {
        if (processor.settings.useStftRoll) {
            stftRoll.setBounds(bounds);
        } else {
            cqtRoll.setBounds(bounds);
        }
    } else if (processor.settings.showEye) {
        eye.setBounds(bounds);
    }
}
