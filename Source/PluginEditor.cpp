#include "PluginProcessor.h"
#include "PluginEditor.h"

PitchengaAudioProcessorEditor::PitchengaAudioProcessorEditor(PitchengaAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), worker(p), lineViz(p) {

    addAndMakeVisible(tunerViz);
    addAndMakeVisible(circleViz);
    addAndMakeVisible(lineViz);

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

        circleViz.updateResults(circleBuffer);
        lineViz.updateResults(lineBuffer);
    }
}

void PitchengaAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black);
}

void PitchengaAudioProcessorEditor::resized() {
    audioProcessor.lastUIWidth = getWidth();
    audioProcessor.lastUIHeight = getHeight();

    auto bounds = getLocalBounds();
    //fixme: WHen both circle and linear are enabled, share height equally
    //fixme: Crashes when circle starts with zero height
    // lineViz.setBounds(bounds);
    lineViz.setBounds(bounds.removeFromTop(LineViz::getPreferredHeight()));
    bounds.removeFromTop(1);
    tunerViz.setBounds(bounds.removeFromBottom(static_cast<int>(TunerViz::getPreferredHeight() + 1)));
    circleViz.setBounds(bounds);
}