#include "Util.h"
#include <iostream>
#include <memory>
#include <vector>

#if JUCE_MAC
#include <pwd.h>
#include <unistd.h>
#endif

#include "version.h"

juce::File Util::getAppFolder() {
#if JUCE_MAC
    // In a sandboxed macOS app, standard JUCE/macOS APIs return the sandboxed Container folder.
    // To access the shared folder granted via temporary-exception, we must get the real home folder using POSIX APIs.
    passwd passwdEntry{};
    passwd* result = nullptr;
    long bufferSize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufferSize == -1) {
        bufferSize = 4096;
    }
    std::vector<char> buffer(static_cast<size_t>(bufferSize));
    if (getpwuid_r(getuid(), &passwdEntry, buffer.data(), buffer.size(), &result) == 0
        && result != nullptr
        && result->pw_dir != nullptr
    ) {
        auto appFolder = juce::File(juce::String(result->pw_dir))
            .getChildFile("Library/Pitchenga");
        return appFolder;
    }
#endif
    auto appFolder = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("Pitchenga");

    return appFolder;
}

static std::once_flag initFlag;

void Util::init() {
    std::call_once(
        initFlag,
        [] {
            static auto activeLogger = std::make_unique<PitchengaLogger>();
            juce::Logger::setCurrentLogger(activeLogger.get());
        }
    );
}

void Util::log(const juce::String& message) {
    juce::Logger::writeToLog(message);
}

Util::PitchengaLogger::PitchengaLogger() {
    startTimestamp = getTimestamp();
    const auto logsFolder = getAppFolder().getChildFile("logs");
    logFile = logsFolder.getChildFile("pitchenga-" + startTimestamp + ".log");

    juce::Thread::launch(
        [logsFolder] {
            if (logsFolder.isDirectory()) {
                const auto ago = juce::Time::getCurrentTime() - juce::RelativeTime::days(1);
                juce::Array<juce::File> logFiles;
                logsFolder.findChildFiles(
                    logFiles,
                    juce::File::findFiles,
                    false,
                    "pitchenga-*.log"
                );
                for (const auto& file : logFiles) {
                    if (file.getLastModificationTime() < ago) {
                        std::cout << (file.deleteFile() ? "Deleted logFile=" : "Failed deleting logFile=") <<
                            file.getFullPathName().toStdString() << std::endl;
                    }
                }
            }
        }
    );

    PitchengaLogger::logMessage("Pitchenga " + juce::String(VERSION));
    PitchengaLogger::logMessage("appFolder=" + getAppFolder().getFullPathName().toStdString());
}

Util::PitchengaLogger::~PitchengaLogger() = default;

juce::String Util::PitchengaLogger::getTimestamp() {
    const auto time = juce::Time::getCurrentTime();
    return time.formatted("%y%m%d-%H%M%S") + "." + juce::String(time.getMilliseconds()).paddedLeft('0', 3);
}

void Util::PitchengaLogger::logMessage(const juce::String& message) {
    const juce::ScopedLock scopedLock(lock);
    const auto fullMessage = "[" + startTimestamp + "][" + getTimestamp() + "] " + message;

    // Still output to standard out so your IDE console works
    std::cout << fullMessage.toStdString() << std::endl;

    if (createFile()) {
        logFile.appendText(fullMessage + "\n");
    }
}

bool Util::PitchengaLogger::createFile() const {
    // Lock is already held by logMessage
    if (!logFile.getParentDirectory().exists()) {
        std::cout << "Creating folder=" << logFile.getParentDirectory().getFullPathName().toStdString() << std::endl;
        if (!logFile.getParentDirectory().createDirectory()) {
            std::cout << "Failed creating folder=" << logFile.getParentDirectory().getFullPathName().toStdString() <<
                std::endl;
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
