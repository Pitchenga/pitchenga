#include "Util.h"
#include <mutex>

#if JUCE_MAC
#include <pwd.h>
#include <unistd.h>
#endif

#include "version.h"

bool Util::debugLogEnabled = true;
// bool Util::debugLogEnabled = false;

juce::String Util::startTimestamp;

juce::File Util::getApplicationDirectory() {
#if JUCE_MAC
    // In a sandboxed macOS app, standard JUCE/macOS APIs return the sandboxed Container directory.
    // To access the shared folder granted via temporary-exception, we must get the real home directory using POSIX APIs.
    passwd* passwd = getpwuid(getuid());
    if (passwd != nullptr && passwd->pw_dir != nullptr) {
        auto applicationDirectory = juce::File(juce::String(passwd->pw_dir))
            .getChildFile("Library/Pitchenga");
        debug("applicationDirectory=" + applicationDirectory.getFullPathName());
        return applicationDirectory;
    }
#endif
    auto applicationDirectory = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("Pitchenga");
    debug("applicationDirectory=" + applicationDirectory.getFullPathName());
    return applicationDirectory;
}

juce::File Util::logFile;

juce::String Util::getTimestamp() {
    const auto time = juce::Time::getCurrentTime();
    return time.formatted("%y%m%d-%H%M%S") + "." + juce::String(time.getMilliseconds()).paddedLeft('0', 3);
}

static std::once_flag initFlag;

void Util::init() {
    std::call_once(
        initFlag,
        []() {
            startTimestamp = getTimestamp();
            const auto logsDirectory = getApplicationDirectory().getChildFile("logs");
            logFile = logsDirectory.getChildFile("pitchenga-" + startTimestamp + ".log");

            juce::Thread::launch(
                [logsDirectory]() {
                    if (logsDirectory.isDirectory()) {
                        const auto ago = juce::Time::getCurrentTime() - juce::RelativeTime::days(2);
                        juce::Array<juce::File> logFiles;
                        logsDirectory.findChildFiles(logFiles, juce::File::findFiles, false, "pitchenga-*.log");
                        for (const auto& file : logFiles) {
                            if (file.getLastModificationTime() < ago) {
                                bool deleted = file.deleteFile();
                                if (!deleted) {
                                    DBG("Failed deleting logFile=" + file.getFullPathName());
                                }
                            }
                        }
                    }
                }
            );
            debug("Pitchenga " + juce::String(VERSION));
        }
    );
}

void Util::debug(const juce::String& message) {
    if (debugLogEnabled) {
        DBG(message);
        const auto fullMessage = "[" + startTimestamp + "][" + getTimestamp() + "] " + message;

        if (createFile()) logFile.appendText(fullMessage + "\n");
    }
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
