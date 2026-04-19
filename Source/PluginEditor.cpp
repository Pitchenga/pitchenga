#include "PluginProcessor.h"
#include "PluginEditor.h"

PitchengaAudioProcessorEditor::PitchengaAudioProcessorEditor(PitchengaAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), worker(p), roll(p), splitterBar(p), controlOverlay(p) {

    addAndMakeVisible(tunaViz);
    addAndMakeVisible(eye);
    addAndMakeVisible(roll);

    addAndMakeVisible(controlOverlay);
    controlOverlay.onVisibilityChanged = [this] { resized(); };

    addAndMakeVisible(splitterBar);
    splitterBar.onDragged = [this] { resized(); };

    roll.setEngine(worker.getCqtEngine());

    circleBuffer.resize(TheEye::totalFoldedBins, 0.0);

    setResizable(true, true);
    setSize(audioProcessor.uiSettings.lastUIWidth, audioProcessor.uiSettings.lastUIHeight);

    worker.startThread(juce::Thread::Priority::high);
    startTimerHz(uiRefreshRateHz);
}

PitchengaAudioProcessorEditor::~PitchengaAudioProcessorEditor() {
    worker.stopThread(2000);
    stopTimer();
}

void PitchengaAudioProcessorEditor::updateVisibilityFromState() {
    controlOverlay.updateVisibilityFromState();
    resized();
}

void PitchengaAudioProcessorEditor::timerCallback() {
    // --- Update the Tuna ---
    // Read the lock-free atomic variable
    const float latestPitchHz = audioProcessor.currentPitchHz.load(std::memory_order_relaxed);

    // Feed it to the tuna visualizer component
    tunaViz.setPitchFrequency(latestPitchHz);

    // --- Update the Circular Visualizer ---
    if (worker.hasNewData()) {
        worker.getCircleResults(circleBuffer);
        worker.getLineResults(lineBuffer);
        worker.clearNewDataFlag();

        if (audioProcessor.uiSettings.showEye) eye.updateResults(circleBuffer);
        if (audioProcessor.uiSettings.showRoll) roll.updateResults(lineBuffer);
    }
}

void PitchengaAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);
}

void PitchengaAudioProcessorEditor::resized() {
    audioProcessor.uiSettings.lastUIWidth = getWidth();
    audioProcessor.uiSettings.lastUIHeight = getHeight();

    auto bounds = getLocalBounds();

    // Give the control bar its own dedicated, non-overlapping space at the top left
    controlOverlay.setBounds(bounds.removeFromTop(24));

    roll.setVisible(audioProcessor.uiSettings.showRoll);
    eye.setVisible(audioProcessor.uiSettings.showEye);
    tunaViz.setVisible(audioProcessor.uiSettings.showTunaViz);

    // Only show the drag bar if both resizable elements are active
    splitterBar.setVisible(audioProcessor.uiSettings.showRoll && audioProcessor.uiSettings.showEye);

    if (audioProcessor.uiSettings.showTunaViz) {
        tunaViz.setBounds(bounds.removeFromBottom(static_cast<int>(TheTuna::getPreferredHeight() + 1)));
    }

    if (audioProcessor.uiSettings.showRoll && audioProcessor.uiSettings.showEye) {
        const int availableHeight = bounds.getHeight();
        const int rollHeight = static_cast<int>(static_cast<float>(availableHeight) * audioProcessor.uiSettings.splitRatio);

        roll.setBounds(bounds.removeFromTop(rollHeight));
        splitterBar.setBounds(bounds.removeFromTop(4)); // Dedicated 4px hit-box for the resizer
        eye.setBounds(bounds);
    } else if (audioProcessor.uiSettings.showRoll) {
        roll.setBounds(bounds);
    } else if (audioProcessor.uiSettings.showEye) {
        eye.setBounds(bounds);
    }
}