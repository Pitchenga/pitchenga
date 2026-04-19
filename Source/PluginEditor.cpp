#include "PluginProcessor.h"
#include "PluginEditor.h"

PitchengaAudioProcessorEditor::PitchengaAudioProcessorEditor(PitchengaAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), worker(p), lineViz(p), splitterBar(p), controlOverlay(p) {

    addAndMakeVisible(tunerViz);
    addAndMakeVisible(circleViz);
    addAndMakeVisible(lineViz);

    addAndMakeVisible(controlOverlay);
    controlOverlay.onVisibilityChanged = [this] { resized(); };

    addAndMakeVisible(splitterBar);
    splitterBar.onDragged = [this] { resized(); };

    lineViz.setEngine(worker.getCqtEngine());

    circleBuffer.resize(Eye::totalFoldedBins, 0.0);

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
    // --- Update the Tuner ---
    // Read the lock-free atomic variable
    const float latestPitchHz = audioProcessor.currentPitchHz.load(std::memory_order_relaxed);

    // Feed it to the tuner visualizer component
    tunerViz.setPitchFrequency(latestPitchHz);

    // --- Update the Circular Visualizer ---
    if (worker.hasNewData()) {
        worker.getCircleResults(circleBuffer);
        worker.getLineResults(lineBuffer);
        worker.clearNewDataFlag();

        if (audioProcessor.uiSettings.showCircleViz) circleViz.updateResults(circleBuffer);
        if (audioProcessor.uiSettings.showLineViz) lineViz.updateResults(lineBuffer);
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

    lineViz.setVisible(audioProcessor.uiSettings.showLineViz);
    circleViz.setVisible(audioProcessor.uiSettings.showCircleViz);
    tunerViz.setVisible(audioProcessor.uiSettings.showTunerViz);

    // Only show the drag bar if both resizable elements are active
    splitterBar.setVisible(audioProcessor.uiSettings.showLineViz && audioProcessor.uiSettings.showCircleViz);

    if (audioProcessor.uiSettings.showTunerViz) {
        tunerViz.setBounds(bounds.removeFromBottom(static_cast<int>(Tuna::getPreferredHeight() + 1)));
    }

    if (audioProcessor.uiSettings.showLineViz && audioProcessor.uiSettings.showCircleViz) {
        const int availableHeight = bounds.getHeight();
        const int lineVizHeight = static_cast<int>(static_cast<float>(availableHeight) * audioProcessor.uiSettings.splitRatio);

        lineViz.setBounds(bounds.removeFromTop(lineVizHeight));
        splitterBar.setBounds(bounds.removeFromTop(4)); // Dedicated 4px hit-box for the resizer
        circleViz.setBounds(bounds);
    } else if (audioProcessor.uiSettings.showLineViz) {
        lineViz.setBounds(bounds);
    } else if (audioProcessor.uiSettings.showCircleViz) {
        circleViz.setBounds(bounds);
    }
}