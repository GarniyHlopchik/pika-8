#pragma once
#include "interface.h"

class LogLevelStr {
public:
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