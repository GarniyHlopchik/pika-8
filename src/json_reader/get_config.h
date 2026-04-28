#pragma once
#include "simdjson.h"
#include <iostream>
#include <string>
#include <vector>
#include <future>
#include "file_resolve/file_system.h"
#include "async_loader.h"
#include "stb_truetype/stb_truetype.h"
#define CONFIG_FILE_PATH "config.json"

struct FontData {
    std::string name;
    std::string path;
    int char_width;
    int char_height;
    std::string charset;
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

    // Default constructor with defaults
    Config() = default;

    // Async loading method
    static std::future<Config> load_async() {
        return AsyncLoader::run_async([]() -> Config {
            Config config; // Start with defaults

            // loading the file asynchronously
            Resource res = FileSystem::get_resource(CONFIG_FILE_PATH);

            if (!res.is_valid()) {
                std::cerr << "Failed to load config.json. Using defaults." << std::endl;
                return config;
            }

            // Parse the JSON
            simdjson::dom::parser parser;
            simdjson::dom::element doc;
            auto error = parser.parse(res.data.get(), res.size).get(doc);
            if (error) {
                std::cerr << "Failed to parse config.json: " << error << ". Using defaults." << std::endl;
                return config;
            }

            // Extract values
            auto title_res = doc["window_title"].get_string();
            if (!title_res.error()) config.window_title = title_res.value();

            auto lua_res = doc["lua_script"].get_string();
            if (!lua_res.error()) config.lua_script = lua_res.value();

            auto w_res = doc["window_width"].get_int64();
            if (!w_res.error()) config.window_width = w_res.value();

            auto h_res = doc["window_height"].get_int64();
            if (!h_res.error()) config.window_height = h_res.value();

            auto vsync_res = doc["VSync"].get_int64();
            if (!vsync_res.error()) config.VSync = vsync_res.value();

            auto show_console_res = doc["show_console"].get_int64();
            if (!show_console_res.error()) config.show_console = show_console_res.value();

            // Extract fonts array
            auto fonts_res = doc["fonts"].get_array();
            if (!fonts_res.error()) {
                for (auto font_elem : fonts_res.value()) {
                    auto font_obj_res = font_elem.get_object();
                    if (font_obj_res.error()) continue;
                    
                    auto font_obj = font_obj_res.value();
                    FontData font;
                    
                    auto name_res = font_obj["name"].get_string();
                    if (!name_res.error()) font.name = name_res.value();

                    auto path_res = font_obj["font_texture"].get_string();
                    if (!path_res.error()) font.path = path_res.value();

                    auto width_res = font_obj["char_width"].get_int64();
                    if (!width_res.error()) font.char_width = width_res.value();

                    auto height_res = font_obj["char_height"].get_int64();
                    if (!height_res.error()) font.char_height = height_res.value();

                    auto charset_res = font_obj["charset"].get_string();
                    if (!charset_res.error()) font.charset = charset_res.value();

                    if (!font.path.empty()) {
                        config.fonts.push_back(font);
                    }
                }
            }

            return config;
        });
    }

private:
    std::string window_title = "Pika-8"; 
    std::string lua_script = "main.lua"; 
    int64_t window_width = 512; 
    int64_t window_height = 512; 
    int64_t VSync = 1;
    int64_t show_console = 1; // 1 to show console, 0 to hide
    std::vector<FontData> fonts; 
};