#include "Util.h"
#include <mutex>
#include <iostream>

#if JUCE_MAC
#include <pwd.h>
#include <unistd.h>
#endif

#include "version.h"

bool Util::debugLogEnabled = true;
// bool Util::debugLogEnabled = false;

juce::String Util::startTimestamp;

juce::File Util::getAppFolder() {
#if JUCE_MAC
    // In a sandboxed macOS app, standard JUCE/macOS APIs return the sandboxed Container folder.
    // To access the shared folder granted via temporary-exception, we must get the real home folder using POSIX APIs.
    passwd* passwd = getpwuid(getuid());
    if (passwd != nullptr && passwd->pw_dir != nullptr) {
        auto appFolder = juce::File(juce::String(passwd->pw_dir))
            .getChildFile("Library/Pitchenga");
        return appFolder;
    }
#endif
    auto appFolder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("Pitchenga");
    debug("appFolder=" + appFolder.getFullPathName());
    return appFolder;
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
            const auto logsFolder = getAppFolder().getChildFile("logs");
            logFile = logsFolder.getChildFile("pitchenga-" + startTimestamp + ".log");

            juce::Thread::launch(
                [logsFolder]() {
                    if (logsFolder.isDirectory()) {
                        const auto ago = juce::Time::getCurrentTime() - juce::RelativeTime::days(2);
                        juce::Array<juce::File> logFiles;
                        logsFolder.findChildFiles(logFiles, juce::File::findFiles, false, "pitchenga-*.log");
                        for (const auto& file : logFiles) {
                            if (file.getLastModificationTime() < ago) {
                                bool deleted = file.deleteFile();
                                if (!deleted) {
                                    debug("Failed deleting logFile=" + file.getFullPathName());
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
        const auto fullMessage = "[" + startTimestamp + "][" + getTimestamp() + "] " + message;
        std::cout << fullMessage.toStdString() << std::endl;

        if (createFile()) logFile.appendText(fullMessage + "\n");
    }
}

bool Util::createFile() {
    if (!logFile.getParentDirectory().exists()) {
        std::cout << "Creating folder=" << logFile.getParentDirectory().getFullPathName().toStdString() << std::endl;
        if (!logFile.getParentDirectory().createDirectory()) {
            std::cout << "Failed creating folder=" << logFile.getParentDirectory().getFullPathName().toStdString() << std::endl;
            return false;
        }
    }
    if (!logFile.exists()) {
        auto result = logFile.create();
        if (!result) {
            std::cout << "Failed creating logFile=" << logFile.getFullPathName().toStdString() << std::endl;
        }
        return result;
    }
    return true;
}
