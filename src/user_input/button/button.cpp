#include "button.h"
#include "gfx/gfx.h"
#include "gfx/sprite/sprite.h"
#include "gfx/text/text.h"
#include "logger/proxy.h"

namespace button {
    // Global button registry - stores all active buttons
    std::unordered_map<unsigned int, std::shared_ptr<ButtonData>> button_registry;

    unsigned int create_button(float x, float y, float width, float height, unsigned int id, unsigned int texture, std::string name) {
        auto btn_data = std::make_shared<ButtonData>();
        btn_data->x = x;
        btn_data->y = y;
        btn_data->width = width;
        btn_data->height = height;
        btn_data->id = id;
        btn_data->texture = texture;
        btn_data->name = name;
        button_registry[id] = btn_data;
        return id;
    }

    void button_set_on_press_callback(unsigned int button_id, sol::function callback) {
        auto it = button_registry.find(button_id);
        if (it != button_registry.end()) {
            it->second->onPressCallback = callback;
        }
    }

    void button_set_on_hold_callback(unsigned int button_id, sol::function callback) {
        auto it = button_registry.find(button_id);
        if (it != button_registry.end()) {
            it->second->onHoldCallback = callback;
        }
    }

    bool button_contains_cursor(unsigned int button_id, float x, float y) {
        auto it = button_registry.find(button_id);
        if (it != button_registry.end()) {
            const auto& btn = it->second;
            return x >= btn->x && x <= btn->x + btn->width && y >= btn->y && y <= btn->y + btn->height;
        }
        return false;
    }

    void button_check_press(unsigned int button_id, InputState state) {
        auto it = button_registry.find(button_id);
        if (it == button_registry.end()) return;

        auto& btn = it->second;
        const double* cursorPos = getRelativeCursorPosition(state);
        
        bool inside_button = button_contains_cursor(button_id, cursorPos[0], cursorPos[1]);

        if ((IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) && 
			inside_button && 
			btn->onPressCallback.valid()) 
			{
            try {
                btn->onPressCallback();
            } catch (const sol::error& e) {
                // TODO: Add proper error logging
            }
        }
    }

    void button_check_hold(unsigned int button_id, InputState state) {
        auto it = button_registry.find(button_id);
        if (it == button_registry.end()) return;

        auto& btn = it->second;
        const double* cursorPos = getRelativeCursorPosition(state);
        
        bool inside_button = button_contains_cursor(button_id, cursorPos[0], cursorPos[1]);
        
        if ((IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) &&
			inside_button && btn->onHoldCallback.valid()) {
            try {
                btn->onHoldCallback();
            } catch (const sol::error& e) {
                // TODO: Add proper error logging
            }
        }
    }

    void button_update_all(InputState state) {
        for (auto& pair : button_registry) {
            button_check_press(pair.first, state);
            button_check_hold(pair.first, state);
        }
    }

    void button_draw(unsigned int button_id, GFX* gfx) {
        auto it = button_registry.find(button_id);
        if (it == button_registry.end() || !gfx) return;

        const auto& btn = it->second;
        
        // Create a temporary sprite for drawing the button
        Sprite* sprite = new Sprite(  	btn->id, nullptr, nullptr, btn->texture, 
										btn->x, btn->y, 
										btn->width, btn->height, 
										{0.0f, 0.0f, 1.0f, 1.0f}, {255, 255, 255, 255}
									);
		Text* text_renderer = new Text(*gfx);
		float text_scale = (btn->width * btn->height ) / 15000.0f; // Adjust scale based on button size
		// LOG(LogLevel::DEBUG, "Button text scale: ", text_scale);
		text_renderer->draw_text(btn->name, btn->x + 5, btn->y + 5, "default", text_scale, {255, 255, 255, 255});
        sprite->draw(gfx);
    }

    void button_draw_all(GFX* gfx) {
        if (!gfx) return;
        
        for (auto& pair : button_registry) {
            button_draw(pair.first, gfx);
        }
    }

    void button_destroy(unsigned int button_id) {
        button_registry.erase(button_id);
    }

    void button_destroy_all() {
        button_registry.clear();
    }
}