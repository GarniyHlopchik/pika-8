#pragma once
#include "interface.h"

class RealLogger : public ILogger  {
public:
    void log(LogLevel level, const std::string& message) override {
        std::cout << "[" << ILogger::levelToString(level) << "] " << message << "\n";
    }
};