#pragma once

#include <juce_core/juce_core.h>

class Util {
public:
    static juce::File logFile;
    static juce::String startTimestamp;

    static void init();
    static juce::String getTimestamp();
    static bool createFile();
    static void log(const juce::String& message);
    static juce::File getAppFolder();
};
