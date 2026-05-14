#pragma once

#include <juce_core/juce_core.h>

class Util {
public:
    static void init();
    static juce::File getAppFolder();
    static void log(const juce::String& message);

    // --- Custom Logger Integration ---
    class PitchengaLogger : public juce::Logger {
    public:
        PitchengaLogger();
        ~PitchengaLogger() override;

        // Override the core JUCE logging method
        void logMessage(const juce::String& message) override;

    private:
        bool createFile() const;
        static juce::String getTimestamp();

        juce::String startTimestamp;
        juce::File logFile;
        juce::CriticalSection lock;
    };
};
