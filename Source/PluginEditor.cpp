#include "PluginProcessor.h"
#include "PluginEditor.h"

PitchengaAudioProcessorEditor::PitchengaAudioProcessorEditor(PitchengaAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), worker(p), tuna(), eye(), stftRoll(p), cqtRoll(p), splitter(p), control(p) {

    addAndMakeVisible(tuna);
    addAndMakeVisible(eye);
    addAndMakeVisible(stftRoll);
    addAndMakeVisible(cqtRoll);

    addAndMakeVisible(control);
    control.onVisibilityChanged = [this] { resized(); };

    addAndMakeVisible(splitter);
    splitter.onDragged = [this] { resized(); };

    circleBuffer.resize(Eye::totalFoldedBins, 0.0);

    setResizable(true, true);
    setSize(audioProcessor.settings.lastUIWidth, audioProcessor.settings.lastUIHeight);

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
    const float latestPitchHz = audioProcessor.currentPitchHz.load(std::memory_order_relaxed);

    // Feed it to the tuna visualizer component
    tuna.setPitchFrequency(latestPitchHz);

    // --- Update the visualizers ---
    if (worker.hasNewData()) {
        worker.getCircleResults(circleBuffer);
        worker.getRollPeaks(rollPeaksBuffer);
        worker.getLineResults(cqtRollBuffer);
        worker.clearNewDataFlag();

        if (audioProcessor.settings.showEye) eye.updateResults(circleBuffer);
        if (audioProcessor.settings.showRoll) {
            if (audioProcessor.settings.useStftRoll) {
                stftRoll.updateResults(rollPeaksBuffer);
            } else {
                cqtRoll.updateResults(cqtRollBuffer);
            }
        }
    }
}

void PitchengaAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);
}

void PitchengaAudioProcessorEditor::resized() {
    audioProcessor.settings.lastUIWidth = getWidth();
    audioProcessor.settings.lastUIHeight = getHeight();

    auto bounds = getLocalBounds();

    // Give the control bar its own dedicated, non-overlapping space at the top left
    control.setBounds(bounds.removeFromTop(static_cast<int>(Control::getPreferredHeight())));

    stftRoll.setVisible(audioProcessor.settings.showRoll && audioProcessor.settings.useStftRoll);
    cqtRoll.setVisible(audioProcessor.settings.showRoll && !audioProcessor.settings.useStftRoll);
    eye.setVisible(audioProcessor.settings.showEye);
    tuna.setVisible(audioProcessor.settings.showTuna);

    // Only show splitter if both resizable elements are active
    splitter.setVisible(audioProcessor.settings.showRoll && audioProcessor.settings.showEye);

    if (audioProcessor.settings.showTuna) {
        tuna.setBounds(bounds.removeFromBottom(static_cast<int>(Needle::getPreferredHeight() + 1)));
    }

    if (audioProcessor.settings.showRoll && audioProcessor.settings.showEye) {
        const int availableHeight = bounds.getHeight();
        const int rollHeight = static_cast<int>(static_cast<float>(availableHeight) * audioProcessor.settings.splitRatio);

        if (audioProcessor.settings.useStftRoll) {
            stftRoll.setBounds(bounds.removeFromTop(rollHeight));
        } else {
            cqtRoll.setBounds(bounds.removeFromTop(rollHeight));
        }
        splitter.setBounds(bounds.removeFromTop(4)); // Dedicated 4px hit-box for the resizer
        eye.setBounds(bounds);
    } else if (audioProcessor.settings.showRoll) {
        if (audioProcessor.settings.useStftRoll) {
            stftRoll.setBounds(bounds);
        } else {
            cqtRoll.setBounds(bounds);
        }
    } else if (audioProcessor.settings.showEye) {
        eye.setBounds(bounds);
    }
}
