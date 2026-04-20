#include "Util.h"

juce::File Util::logFile = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("pitchenga.log");

void Util::init() {
    // if (logFile.existsAsFile()) {
        // logFile.deleteFile();
    // }
    debug("Pitchenga started, logFile=" + logFile.getFullPathName());
}

void Util::debug(const juce::String& message) {
#if DEBUG
    const auto timestamp = juce::Time::getCurrentTime().formatted("%H:%M:%S:%L");
    const auto fullMessage = "[" + timestamp + "] " + message;
    DBG(fullMessage);
    logFile.appendText(fullMessage + "\n");
#else
    juce::ignoreUnused(message);
#endif
}
