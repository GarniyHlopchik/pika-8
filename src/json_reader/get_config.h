#include "json_reader.h"

#define CONFIG_FILE_PATH "config.json"

class Config {
public:
    ~Config();
    int64_t get_window_width() const { return window_width; }
    int64_t get_window_height() const { return window_height; }
    std::string get_window_title() const { return window_title; }
    std::string get_lua_script() const { return lua_script; }

    Config() {
        if(get_json_element_by_key(CONFIG_FILE_PATH, "window_title", elem)) {
            window_title = elem.get_string().value();
        }
        
        if (get_json_element_by_key(CONFIG_FILE_PATH, "lua_script", elem)) {
            lua_script = elem.get_string().value();
        }

        if (get_json_element_by_key(CONFIG_FILE_PATH, "window_width", elem)) {
            window_width = elem.get_int64().value();
        }

        if (get_json_element_by_key(CONFIG_FILE_PATH, "window_height", elem)) {
            window_height = elem.get_int64().value();
        }
    }

private:
    simdjson::dom::element elem;

    std::string window_title = "Pika-67"; // Default to "Pika-8" if not specified
    std::string lua_script = "main.lua"; // Default to "main.lua" if not specified
    int64_t window_width = 512; // Default to 512 if not specified
    int64_t window_height = 512; // Default to 512 if not specified

    
};

inline Config::~Config(){
    free((void*)window_title.c_str());
    free((void*)lua_script.c_str());
    delete this;
};