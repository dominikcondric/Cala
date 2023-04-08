#include "Logger.h"
#include <iostream>

namespace Cala {
    Logger& Logger::getInstance()
    {
        static Logger instance;
        return instance;
    }

    void Logger::logErrorToConsole(std::string_view message) const
    {
        std::cerr << "[ERROR]: " << message << std::endl;
    }

    void Logger::logInfoToConsole(std::string_view message) const
    {
        std::cout << "[INFO]: " << message << std::endl;
    }

    void Logger::logDebugToConsole(std::string_view message) const
    {
        std::clog << "[DEBUG]: " << message << std::endl;
    }
}