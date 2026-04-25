#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "math/Math.h"
#include "ui/Needle.h"
#include "ui/Eye.h"
#include "ui/RollStft.h"
#include "ui/RollCqt.h"
#include "ui/Control.h"
#include "ui/Splitter.h"

class PitchengaAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    explicit PitchengaAudioProcessorEditor(PitchengaAudioProcessor&);
    ~PitchengaAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void updateVisibilityFromState();
    void openPluginWindow();
    void openPluginBrowserWindow();
    void startPluginScan();

    bool isPluginWindowOpen() const { return pluginWindow != nullptr && pluginWindow->isVisible(); }

private:
    void timerCallback() override;

    PitchengaAudioProcessor& processor;

    // Background worker for heavy DSP
    Math worker;

    Needle needle;
    Eye eye;
    RollStft rollStft;
    RollCqt rollCqt;
    Splitter splitter;

    Control control;

    // Shared results for rendering
    std::vector<double> eyeBuffer;
    std::vector<SpectralPeak> rollStftBuffer;
    std::vector<double> rollCqtBuffer;

    static constexpr int uiRefreshRateHz = 48;

    std::unique_ptr<juce::DocumentWindow> pluginWindow;
    std::unique_ptr<juce::DocumentWindow> browserWindow;
    juce::PluginListComponent* listComponent = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchengaAudioProcessorEditor)
};
