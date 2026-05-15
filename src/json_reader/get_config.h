#pragma once
#include "simdjson.h"
#include <iostream>
#include <string>
#include <vector>
#include "file_resolve/file_system.h"
#include "stb_truetype/stb_truetype.h"
#define CONFIG_FILE_PATH "config.json"

struct FontData {
    std::string name;
    std::string path;

    GLuint textureID;
    stbtt_bakedchar cdata[96]; // ASCII 32..126
};

enum LogLevel {
	ALL = 0,
	EROR = 1, // EROR and not ERROR because something defines ERROR an 0
	WARNING = 2,
	INFO = 3,
	DEBUG = 4
};

struct LoggerData {
	int64_t is_logging = 0; // 1 = enable logging || 0 = disable logging (console)

	int64_t save_log_in_file = 0; // 0 = save logs in file || 1 = don't save logs in file
	std::string log_file_path = "logs/pika8.log";
	
	LogLevel log_level = LogLevel::EROR;
	//  "all"  ||  "error"  ||   "warning" ||  "info"  ||  "debug"

};


class Config {
public:
    ~Config() = default;

    int64_t get_window_width() const { return window_width; }
    int64_t get_window_height() const { return window_height; }
    std::string get_window_title() const { return window_title; }
    std::string get_lua_script() const { return lua_script; }
    int64_t get_vsync() const { return VSync; }
    int64_t get_show_console() const { return show_console; }
    const std::vector<FontData>& get_fonts() const { return fonts; }
    const LoggerData& get_logger_data() const { return logger_data; }

    Config() {
        // create ONE parser for the whole configuration
        simdjson::dom::parser parser;
        simdjson::dom::element doc;
        
        // load the file
        Resource res = FileSystem::get_resource(CONFIG_FILE_PATH);

        if (!res.is_valid()) {
            std::cerr << "Failed to load config.json. Using defaults." << std::endl;
            return;
        }
        //we now parse by data, not just load from file
        auto error = parser.parse(res.data.get(), res.size).get(doc);
        if (error) {
            std::cerr << "Failed to load config.json: " << error << ". Using defaults." << std::endl;
            return; 
        }




        // Extract values (if they are missing or wrong type, it just skips and keeps the default)
        auto title_res = doc["window_title"].get_string();
        if (!title_res.error()) window_title = title_res.value();

        auto lua_res = doc["lua_script"].get_string();
        if (!lua_res.error()) lua_script = lua_res.value();

        auto w_res = doc["window_width"].get_int64();
        if (!w_res.error()) window_width = w_res.value();

        auto h_res = doc["window_height"].get_int64();
        if (!h_res.error()) window_height = h_res.value();

        auto vsync_res = doc["VSync"].get_int64();
        if (!vsync_res.error()) VSync = vsync_res.value();

        auto show_console_res = doc["show_console"].get_int64();
        if (!show_console_res.error()) show_console = show_console_res.value();

        // extract the fonts array
        auto fonts_res = doc["fonts"].get_array();
        if (!fonts_res.error()) {
            for (auto font_elem : fonts_res.value()) {
                auto font_obj_res = font_elem.get_object();
                if (font_obj_res.error()) continue;
                
                auto font_obj = font_obj_res.value();
                FontData font;
                
                auto name_res = font_obj["name"].get_string();
                if (!name_res.error()) font.name = name_res.value();

                auto path_res = font_obj["path"].get_string();
                if (!path_res.error()) font.path = path_res.value();


                if (!font.path.empty()) {
                    fonts.push_back(font);
                    // std::cout << "Successfully loaded font: " << font.name << std::endl;
                }
            }
        }

		// extract logger config
		auto logger_res = doc["logger"].get_object();
		if (!logger_res.error()) {
			auto logger_obj = logger_res.value();

			auto is_logging_res = logger_obj["is_logging"].get_int64();
			if (!is_logging_res.error()) logger_data.is_logging = is_logging_res.value();

			auto save_log_in_file_res = logger_obj["save_log_in_file"].get_int64();
			if (!save_log_in_file_res.error()) logger_data.save_log_in_file = save_log_in_file_res.value();
			
			if (logger_data.save_log_in_file) {
				auto log_file_path_res = logger_obj["log_file_path"].get_string();
				if (!log_file_path_res.error()) logger_data.log_file_path = log_file_path_res.value();
			} else {
				logger_data.log_file_path = "";
			}

			auto log_level_res = logger_obj["log_level"].get_int64();
			if (!log_level_res.error()) {
				// we handle it as an enum of values
				switch (log_level_res.value()) {
					case 0: logger_data.log_level = LogLevel::ALL; break;
					case 1: logger_data.log_level = LogLevel::EROR; break;
					case 2: logger_data.log_level = LogLevel::WARNING; break;
					case 3: logger_data.log_level = LogLevel::INFO; break;
					case 4: logger_data.log_level = LogLevel::DEBUG; break;

					default: logger_data.log_level = LogLevel::ALL; break;
				}
			}
			
			std::cout << "Logger config - is_logging: " << static_cast<int>(logger_data.is_logging) 
					  << ", save_log_in_file: " << static_cast<int>(logger_data.save_log_in_file) 
					  << ", log_file_path: " << logger_data.log_file_path 
					  << ", log_level: " << static_cast<int>(logger_data.log_level) << std::endl;

		}

    }

private:
	// WINDOW CONFIG
    std::string window_title = "Pika-8"; 
    std::string lua_script = "main.lua"; 
    int64_t window_width = 512; 
    int64_t window_height = 512;
    std::vector<FontData> fonts; 

	// RENDER CONFIG
    int64_t VSync = 1;
    int64_t show_console = 1; // 1 = show console || 0 = hide console

	// LOGGER
	LoggerData logger_data;

};
