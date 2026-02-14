#include "json_reader.h"

class Config {
public:
    int64_t get_window_width() const { return window_width; }
    int64_t get_window_height() const { return window_height; }
    std::string get_window_title() const { return window_title; }
    std::string get_lua_script() const { return lua_script; }

private:
    std::string window_title = get_json_element_by_key("config.json", "window_title").get_string().value().data();
    std::string lua_script = get_json_element_by_key("config.json", "lua_script").get_string().value().data();
    int64_t window_width = get_json_element_by_key("config.json", "window_width").get_int64().value();
    int64_t window_height = get_json_element_by_key("config.json", "window_height").get_int64().value();
};
