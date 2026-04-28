#include "user_input.h"
#include "gfx/gfx.h"
#include <algorithm>
#include <iostream>

//check if key was just pressed
bool IsKeyPressed(InputState state, int key)
{
    return (state.keys[key] && !state.previous_keys[key]); //pressed now but wasn't last frame
}

//check if key is pressed
bool IsKeyDown(InputState state, int key)
{
    return state.keys[key]; 

}


double* getRelativeCursorPosition(InputState state){
    double* position = new double[2];
    position[0] = state.mouseX;
    position[1] = state.mouseY;
    return position;
}

static mobile_input::InputState touch_state;
static int touch_window_width = 0;
static int touch_window_height = 0;

void initTouchInput() {
    touch_state.clear();
    std::cout << "[INPUT] Touch system initialized" << std::endl;
}

void setTouchWindowSize(int width, int height) {
    touch_window_width = width;
    touch_window_height = height;
    std::cout << "[INPUT] Window size set to " << width << "x" << height << std::endl;
}

mobile_input::InputState* getTouchState() {
    return &touch_state;
}

void resetTouchFrame() {
    touch_state.resetFrame();
}

void handleTouchEvent(const SDL_Event& event) {
    const auto& tf = event.tfinger;

    if (touch_window_width == 0 || touch_window_height == 0) {
        return;
    }

    switch (event.type) {
    case SDL_EVENT_FINGER_DOWN: {
        mobile_input::TouchPoint t;
        t.id = tf.fingerID;
        t.button = 0; 
        t.x = tf.x * touch_window_width;
        t.y = tf.y * touch_window_height;
        t.down = true;
        t.just_pressed = true;
        t.timestamp = SDL_GetTicks();
        touch_state.touches.push_back(t);
        touch_state.num_touches++;
        break;
    }

    case SDL_EVENT_FINGER_UP: {
        for (auto& t : touch_state.touches) {
            if (t.id == tf.fingerID) {
                t.down = false;
                t.just_released = true;
                break;
            }
        }
       
        auto it = std::remove_if(touch_state.touches.begin(),
            touch_state.touches.end(),
            [](const mobile_input::TouchPoint& p) { return !p.down; });
        touch_state.touches.erase(it, touch_state.touches.end());
        touch_state.num_touches = touch_state.touches.size();
        break;
    }

    case SDL_EVENT_FINGER_MOTION: {
        for (auto& t : touch_state.touches) {
            if (t.id == tf.fingerID) {
                float new_x = tf.x * touch_window_width;
                float new_y = tf.y * touch_window_height;
                t.dx = new_x - t.x;
                t.dy = new_y - t.y;
                t.x = new_x;
                t.y = new_y;
                break;
            }
        }
        break;
    }
    }
}

void handleMouseEvent(const SDL_Event& event) {
    static bool mouse_left_down = false;
    static bool mouse_right_down = false;

    switch (event.type) {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
            mouse_left_down = true;
            mobile_input::TouchPoint t;
            t.id = 999;
            t.button = 1;  
            t.x = event.button.x;
            t.y = event.button.y;
            t.down = true;
            t.just_pressed = true;
            t.timestamp = SDL_GetTicks();
            touch_state.touches.push_back(t);
            touch_state.num_touches++;
            touch_state.is_emulating = true;
            printf("[MOUSE] Left DOWN at (%.0f, %.0f)\n", t.x, t.y);
        }
        else if (event.button.button == SDL_BUTTON_RIGHT) {
            mouse_right_down = true;
            mobile_input::TouchPoint t;
            t.id = 998;
            t.button = 2; 
            t.x = event.button.x;
            t.y = event.button.y;
            t.down = true;
            t.just_pressed = true;
            t.timestamp = SDL_GetTicks();
            touch_state.touches.push_back(t);
            touch_state.num_touches++;
            touch_state.is_emulating = true;
            printf("[MOUSE] Right DOWN at (%.0f, %.0f)\n", t.x, t.y);
        }
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (event.button.button == SDL_BUTTON_LEFT) {
            mouse_left_down = false;
            for (auto& t : touch_state.touches) {
                if (t.id == 999) {
                    t.down = false;
                    t.just_released = true;
                    break;
                }
            }
            auto it = std::remove_if(touch_state.touches.begin(),
                touch_state.touches.end(),
                [](const mobile_input::TouchPoint& p) { return !p.down; });
            touch_state.touches.erase(it, touch_state.touches.end());
            touch_state.num_touches = touch_state.touches.size();
            printf("[MOUSE] Left UP\n");
        }
        else if (event.button.button == SDL_BUTTON_RIGHT) {
            mouse_right_down = false;
            for (auto& t : touch_state.touches) {
                if (t.id == 998) {
                    t.down = false;
                    t.just_released = true;
                    break;
                }
            }
            auto it = std::remove_if(touch_state.touches.begin(),
                touch_state.touches.end(),
                [](const mobile_input::TouchPoint& p) { return !p.down; });
            touch_state.touches.erase(it, touch_state.touches.end());
            touch_state.num_touches = touch_state.touches.size();
            printf("[MOUSE] Right UP\n");
        }
        break;

    case SDL_EVENT_MOUSE_MOTION:
        if (mouse_left_down) {
            for (auto& t : touch_state.touches) {
                if (t.id == 999) {
                    t.dx = event.motion.x - t.x;
                    t.dy = event.motion.y - t.y;
                    t.x = event.motion.x;
                    t.y = event.motion.y;
                    break;
                }
            }
        }
        if (mouse_right_down) {
            for (auto& t : touch_state.touches) {
                if (t.id == 998) {
                    t.dx = event.motion.x - t.x;
                    t.dy = event.motion.y - t.y;
                    t.x = event.motion.x;
                    t.y = event.motion.y;
                    break;
                }
            }
        }
        break;
    }
}
