#pragma once
#include "json_reader/get_config.h"
#include "colors.h"

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void log(LogLevel level, const std::string& message) = 0;
};