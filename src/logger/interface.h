#pragma once
#include "json_reader/get_config.h"


// Abstract interface — both Real and Proxy implement this
class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void log(LogLevel level, const std::string& message) = 0;

	static std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::ALL:     return "ALL";
            case LogLevel::EROR:    return "ERROR";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::INFO:    return "INFO";
            case LogLevel::DEBUG:   return "DEBUG";
            default:                return "UNKNOWN";
        }
    }
};