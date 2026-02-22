#pragma once
#include "simdjson.h"
#include <iostream>
#include <string>
#include <vector>

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
    const std::vector<FontData>& get_fonts() const { return fonts; }

    Config() {
        // 1. Create ONE parser for the whole configuration
        simdjson::dom::parser parser;
        simdjson::dom::element doc;
        
        // 2. Load the file ONCE
        auto error = parser.load(CONFIG_FILE_PATH).get(doc);
        if (error) {
            std::cerr << "Failed to load config.json: " << error << ". Using defaults." << std::endl;
            return; 
        }

        // 3. Safely extract values (if they are missing or wrong type, it just skips and keeps the default)
        auto title_res = doc["window_title"].get_string();
        if (!title_res.error()) window_title = title_res.value();

        auto lua_res = doc["lua_script"].get_string();
        if (!lua_res.error()) lua_script = lua_res.value();

        auto w_res = doc["window_width"].get_int64();
        if (!w_res.error()) window_width = w_res.value();

        auto h_res = doc["window_height"].get_int64();
        if (!h_res.error()) window_height = h_res.value();

        // 4. Safely extract the fonts array
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
                    fonts.push_back(font);
                    // std::cout << "Successfully loaded font: " << font.name << std::endl;
                }
            }
        }
    }

private:
    std::string window_title = "Pika-8"; 
    std::string lua_script = "main.lua"; 
    int64_t window_width = 512; 
    int64_t window_height = 512; 
    std::vector<FontData> fonts; 
};