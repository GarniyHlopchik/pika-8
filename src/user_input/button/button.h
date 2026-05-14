#pragma once

#include "user_input/user_input.h"
#include <sol/sol.hpp>
#include <unordered_map>
#include <memory>

class GFX;

namespace button {
    struct ButtonData {
        float x, y, width, height;
        unsigned int id;
        unsigned int texture;
		std::string name;
        sol::function onPressCallback;
        sol::function onHoldCallback;
    };

    // Global button registry
    extern std::unordered_map<unsigned int, std::shared_ptr<ButtonData>> button_registry;

    // Function-based API
    unsigned int create_button(float x, float y, float width, float height, unsigned int id, unsigned int texture = 0, std::string name = "");
    void button_set_on_press_callback(unsigned int button_id, sol::function callback);
    void button_set_on_hold_callback(unsigned int button_id, sol::function callback);
    void button_check_press(unsigned int button_id, InputState state);
    void button_check_hold(unsigned int button_id, InputState state);
    void button_update_all(InputState state);
    void button_draw(unsigned int button_id, GFX* gfx);
    void button_draw_all(GFX* gfx);
    void button_destroy(unsigned int button_id);
    void button_destroy_all();
    bool button_contains_cursor(unsigned int button_id, float x, float y);
}
