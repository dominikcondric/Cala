#pragma once
#include <filesystem>

namespace Cala {
    class Logger {
    public:
        enum class Level {
            Info,
            Debug,
            Error,
        };

        ~Logger() = default;
        Logger(const Logger&) = delete;
        static Logger& getInstance();
        // void logToFile(const std::string& message, Level level);
        void logErrorToConsole(std::string_view message) const;
        void logInfoToConsole(std::string_view message) const;
        void logDebugToConsole(std::string_view message) const;

    private:
        Logger() = default;
        std::filesystem::path loggingFile;
    };
}