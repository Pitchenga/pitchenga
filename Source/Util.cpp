#include "Util.h"
#include "build_timestamp.h"

juce::File Util::logFile = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
    .getChildFile("pitchenga")
    .getChildFile("pitchenga.log");

void Util::init() {
    DBG("Starting Pitchenga");
#if JUCE_DEBUG
    if (logFile.exists()) {
        if (logFile.deleteFile()) {
            debug("Deleted logFile=" + logFile.getFullPathName());
        } else {
            debug("Failed deleting logFile=" + logFile.getFullPathName());
        }
    }
    debug("Pitchenga started, logFile=" + logFile.getFullPathName() + ", build=" + BUILD_TIMESTAMP);
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

void Util::debug(const juce::String& message) {
#if JUCE_DEBUG
    DBG(message);
    const auto time = juce::Time::getCurrentTime();
    const auto timestamp = time.formatted("%H:%M:%S") + "." + juce::String(time.getMilliseconds()).paddedLeft('0', 3);
    const auto fullMessage = "[" + timestamp + "] " + message;

    // Ensure file exists and append
    if (createFile()) logFile.appendText(fullMessage + "\n");
#else
    juce::ignoreUnused(message);
#endif
}
