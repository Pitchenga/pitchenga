#include "Util.h"
#include "version.h"

bool Util::debugLogEnabled = true;
// bool Util::debugLogEnabled = false;

juce::String Util::startTimestamp = getTimestamp();

juce::File Util::getApplicationDirectory() {
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("Pitchenga");
}

juce::File Util::logFile = getApplicationDirectory()
    .getChildFile("logs")
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
    const auto logsDirectory = getApplicationDirectory().getChildFile("logs");
    if (logsDirectory.isDirectory()) {
        const auto oneWeekAgo = juce::Time::getCurrentTime() - juce::RelativeTime::weeks(1);
        juce::Array<juce::File> logFiles;
        logsDirectory.findChildFiles(logFiles, juce::File::findFiles, false, "pitchenga-*.log");
        for (const auto& file : logFiles) {
            if (file.getLastModificationTime() < oneWeekAgo) {
                bool deleted = file.deleteFile();
                if (!deleted) {
                    DBG("Failed deleting logFile=" + file.getFullPathName());
                }
            }
        }
    }

#if JUCE_DEBUG
    if (debugLogEnabled && logFile.exists()) {
        bool deleted = logFile.deleteFile();
        if (!deleted) {
            DBG("Failed deleting logFile=" + logFile.getFullPathName());
        }
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
