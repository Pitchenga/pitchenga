#include "PluginProcessor.h"
#include "PluginEditor.h"

PitchengaAudioProcessorEditor::PitchengaAudioProcessorEditor(PitchengaAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), worker(p), lineViz(p), controlOverlay(p) {

    addAndMakeVisible(tunerViz);
    addAndMakeVisible(circleViz);
    addAndMakeVisible(lineViz);

    addAndMakeVisible(controlOverlay);
    controlOverlay.onVisibilityChanged = [this] { resized(); };

    lineViz.setEngine(worker.getCqtEngine());

    circleBuffer.resize(CircleViz::totalFoldedBins, 0.0);

    setResizable(true, true);
    setSize(audioProcessor.lastUIWidth, audioProcessor.lastUIHeight);

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

        if (audioProcessor.showCircleViz) circleViz.updateResults(circleBuffer);
        if (audioProcessor.showLineViz) lineViz.updateResults(lineBuffer);
    }
}

void PitchengaAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);
}

void PitchengaAudioProcessorEditor::resized() {
    audioProcessor.lastUIWidth = getWidth();
    audioProcessor.lastUIHeight = getHeight();

    auto bounds = getLocalBounds();

    controlOverlay.setBounds(bounds.removeFromTop(24));

    lineViz.setVisible(audioProcessor.showLineViz);
    circleViz.setVisible(audioProcessor.showCircleViz);
    tunerViz.setVisible(audioProcessor.showTunerViz);

    if (audioProcessor.showTunerViz) {
        tunerViz.setBounds(bounds.removeFromBottom(static_cast<int>(TunerViz::getPreferredHeight() + 1)));
    }

    if (audioProcessor.showLineViz && audioProcessor.showCircleViz) {
        lineViz.setBounds(bounds.removeFromTop(bounds.getHeight() / 2));
        circleViz.setBounds(bounds);
    } else if (audioProcessor.showLineViz) {
        lineViz.setBounds(bounds);
    } else if (audioProcessor.showCircleViz) {
        circleViz.setBounds(bounds);
    }
}