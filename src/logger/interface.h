#pragma once
#include "json_reader/get_config.h"
#include "colors.h"

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void log(LogLevel level, const std::string& message) = 0;

	static std::string levelToString(LogLevel level, bool colored = true) {
	if (!colored) {
		switch (level) {
			case LogLevel::ALL:     return "ALL";
			case LogLevel::EROR:    return "ERROR";
			case LogLevel::WARNING: return "WARNING";
			case LogLevel::INFO:    return "INFO";
			case LogLevel::DEBUG:   return "DEBUG";
			default:                return "UNKNOWN";
		}
		}
	switch (level) {
			case LogLevel::ALL:     return BOLD  WHITE   "ALL"     RESET;
			case LogLevel::EROR:    return BOLD  RED     "ERROR"   RESET;
			case LogLevel::WARNING: return BOLD  YELLOW  "WARNING" RESET;
			case LogLevel::INFO:    return BOLD  GREEN   "INFO"    RESET;
			case LogLevel::DEBUG:   return BOLD  CYAN    "DEBUG"   RESET;
			default:                return BOLD  MAGENTA "UNKNOWN" RESET;
	}
	}
};