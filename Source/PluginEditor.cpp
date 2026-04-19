#include "PluginProcessor.h"
#include "PluginEditor.h"

PitchengaAudioProcessorEditor::PitchengaAudioProcessorEditor(PitchengaAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), worker(p), lineViz(p) {

    addAndMakeVisible(tunerViz);
    addAndMakeVisible(circleViz);
    addAndMakeVisible(lineViz);

    setupToggleButton(toggleLineViz, audioProcessor.showLineViz);
    toggleLineViz.onClick = [this] {
        audioProcessor.showLineViz = toggleLineViz.getToggleState();
        resized();
    };

    setupToggleButton(toggleCircleViz, audioProcessor.showCircleViz);
    toggleCircleViz.onClick = [this] {
        audioProcessor.showCircleViz = toggleCircleViz.getToggleState();
        resized();
    };

    setupToggleButton(toggleTunerViz, audioProcessor.showTunerViz);
    toggleTunerViz.onClick = [this] {
        audioProcessor.showTunerViz = toggleTunerViz.getToggleState();
        resized();
    };

    addAndMakeVisible(toggleLineViz);
    addAndMakeVisible(toggleCircleViz);
    addAndMakeVisible(toggleTunerViz);

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

void PitchengaAudioProcessorEditor::setupToggleButton(juce::TextButton& button, bool initialState) {
    button.setClickingTogglesState(true);
    button.setToggleState(initialState, juce::NotificationType::dontSendNotification);
    button.setColour(juce::TextButton::buttonColourId, juce::Colours::black.withAlpha(0.4f));
    button.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white.withAlpha(0.2f));
    button.setColour(juce::TextButton::textColourOffId, juce::Colours::grey);
    button.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
}

void PitchengaAudioProcessorEditor::updateVisibilityFromState() {
    toggleLineViz.setToggleState(audioProcessor.showLineViz, juce::NotificationType::dontSendNotification);
    toggleCircleViz.setToggleState(audioProcessor.showCircleViz, juce::NotificationType::dontSendNotification);
    toggleTunerViz.setToggleState(audioProcessor.showTunerViz, juce::NotificationType::dontSendNotification);
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

    // Position translucent toggle overlay independently of the main bounds
    juce::Rectangle<int> buttonArea(getWidth() - 190, 10, 180, 24);
    toggleTunerViz.setBounds(buttonArea.removeFromRight(60).reduced(2));
    toggleCircleViz.setBounds(buttonArea.removeFromRight(60).reduced(2));
    toggleLineViz.setBounds(buttonArea.removeFromRight(60).reduced(2));

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