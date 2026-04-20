#include "Util.h"
#include "build_timestamp.h"

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
    DBG("[" + startTimestamp + "] Starting Pitchenga");
#if JUCE_DEBUG
    //fixme: Clean-up old log files
    if (logFile.exists()) {
        if (logFile.deleteFile()) {
            debug("Deleted logFile=" + logFile.getFullPathName());
        } else {
            debug("Failed deleting logFile=" + logFile.getFullPathName());
        }
    }
    debug("Pitchenga started, logFile=" + logFile.getFullPathName() + ", build=" + juce::String(BUILD_TIMESTAMP));
#endif
}

void Util::debug(const juce::String& message) {
#if JUCE_DEBUG
    DBG(message);
    const auto fullMessage = "[" + startTimestamp + "][" + getTimestamp() + "] " + message;

    if (createFile()) logFile.appendText(fullMessage + "\n");
#else
    juce::ignoreUnused(message);
#endif
}

bool Util::createFile() {
    if (!logFile.getParentDirectory().exists()) {
        if (!logFile.getParentDirectory().createDirectory()) {
            return false;
        }
    }
    if (!logFile.exists()) {
        return logFile.create();
    }
    return true;
}