#include "Util.h"
#include "build_timestamp.h"

bool Util::debugLogEnabled = true;
// bool Util::debugLogEnabled = false;

juce::String Util::startTimestamp = getTimestamp();

juce::File Util::logFile = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
    .getChildFile("pitchenga")
    .getChildFile("pitchenga-" + startTimestamp + ".log");


[[maybe_unused]] static struct UtilInitializer {
    UtilInitializer() {
        Util::init();
    }
} utilInitializer;

juce::String Util::getTimestamp() {
    const auto time = juce::Time::getCurrentTime();
    return time.formatted("%H.%M.%S") + "." + juce::String(time.getMilliseconds()).paddedLeft('0', 3);
}

void Util::init() {
#if JUCE_DEBUG
    //fixme: Clean-up old log files
    if (debugLogEnabled && logFile.exists()) {
        logFile.deleteFile();
    }
#endif
}

void Util::debug(const juce::String& message) {
#if JUCE_DEBUG
    if (debugLogEnabled) {
        DBG(message);
        const auto fullMessage = "[" + startTimestamp + "][" + getTimestamp() + "] " + message;

        if (createFile()) logFile.appendText(fullMessage + "\n");
    }
#else
    juce::ignoreUnused(message);
#endif
}

bool Util::createFile() {
    if (!logFile.getParentDirectory().exists()) {
        if (!logFile.getParentDirectory().createDirectory()) {
            DBG("Failed creating directory=" + logFile.getParentDirectory().getFullPathName());
            return false;
        }
    }
    if (!logFile.exists()) {
        auto result = logFile.create();
        if (!result) {
            DBG("Failed creating logFile=" + logFile.getFullPathName());
        }
        return result;
    }
    return true;
}