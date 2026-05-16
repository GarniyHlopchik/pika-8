#pragma once
#include "interface.h"
#include <time.h>
#include "debug/framecounter.h"

class LogFileSave : public ILogger {
public:
	LogFileSave(const LoggerData& config) : _config(config) {};

	void log(LogLevel level, const std::string& message) override {
		if (_config.save_log_in_file == 0) { return; }

		const std::string& file = _config.log_file_path.empty() 
			? "logs/pika8.log" 
			: _config.log_file_path;
		
		create_nonexistant_directories(file);
		std::ofstream log_file(file, std::ios_base::app);

		// DATE FORMATING TO EXCLUDE \n at the end
		struct tm* ptr;
		time_t t;
		t = time(NULL);
		ptr = localtime(&t);
		char date[128];
		strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", ptr);
		frame = FrameCounter::get_frame_count();
		if (log_file.is_open()) {
			log_file << date << "-" << frame << " [" << ILogger::levelToString(level, false) << "] " << message << "\n";
			log_file.close();
		}
	}

private:
	LoggerData _config;
	static inline uint64_t frame = FrameCounter::get_frame_count();

	void create_nonexistant_directories(const std::string& path) {
		size_t pos = 0;
		while ((pos = path.find('/', pos)) != std::string::npos) {
			std::string dir = path.substr(0, pos);
			if (!dir.empty() && !std::filesystem::exists(dir)) {
				std::filesystem::create_directory(dir);
			}
			pos++;
		}
	}

};