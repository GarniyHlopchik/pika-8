#pragma once
#include "interface.h"
#include "log_level_str.h"

class RealLogger : public ILogger, public LogLevelStr {
public:
    void log(LogLevel level, const std::string& message) override {
        std::cout << "[" << LogLevelStr::levelToString(level) << "] " << message << "\n";
    }
};