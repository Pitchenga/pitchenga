#pragma once

#include <juce_core/juce_core.h>

class Util {
public:
    static juce::File logFile;

    static void init();
    static void debug(const juce::String& message);
};
