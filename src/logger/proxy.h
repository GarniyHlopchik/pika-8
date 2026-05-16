#pragma once
#include "real.h"
#include "log_file_save.h"

#define LOG(level, ...) ProxyLogger::getInstance().log(level, __VA_ARGS__)

class ProxyLogger : public ILogger {
public:
	static ProxyLogger& getInstance() {
        static ProxyLogger instance;  // created once, lives forever
        return instance;
    }

	ProxyLogger(const ProxyLogger&) = delete;
	ProxyLogger& operator=(const ProxyLogger&) = delete;

	void init(const LoggerData& config) {
        _config = config;
		_file = std::make_unique<LogFileSave>(_config);
    }

	void log(LogLevel level, const std::string& message) override {
		if(_file) { _file->log(level, message); }
		
		if (_config.is_logging == 0) { return; }
		if (!shouldLog(level)) { return; }

		_real->log(level, message);
	}

	template<typename... Args>
	void log(LogLevel level, Args&&... args) {
		std::ostringstream msg;
		(msg << ... << std::forward<Args>(args));
		
		if(_file) { _file->log(level, msg.str()); }
		
		if (!shouldLog(level)) { return; }
		if (_config.is_logging == 0) { return; }


		_real->log(level, msg.str());
	}

    void setConfig(const LoggerData& config) {
        _config = config;
		_file = std::make_unique<LogFileSave>(_config);
    }

    const LoggerData& getConfig() const {
        return _config;
    }

private:
	ProxyLogger()
		: _real(std::make_unique<RealLogger>())
		, _file(nullptr)
	{}

    bool shouldLog(LogLevel level) const {
        if (_config.log_level == LogLevel::ALL) { return true; }
        if (_config.log_level == LogLevel::NONE) { return false; }
		
        return level == _config.log_level;
    }

    static void printLevelBanner(LogLevel level) {
        std::cout << ">>> Logging level: " << ILogger::levelToString(level) << " <<<\n";
    }

    LoggerData _config;
    std::unique_ptr<RealLogger> _real;
    std::unique_ptr<LogFileSave> _file;
};