#pragma once
#include <vector>

namespace mobile_input {

    struct TouchPoint {
        int id;
        float x, y;
        float dx, dy;
        bool down;
        bool just_pressed;
        bool just_released;
        unsigned int timestamp;
        int button;
    };

    struct InputState {
        std::vector<TouchPoint> touches;
        int num_touches = 0;
        bool is_emulating = false; 

        void resetFrame() {
            for (auto& t : touches) {
                t.just_pressed = false;
                t.just_released = false;
                t.dx = 0;
                t.dy = 0;
            }
            is_emulating = false; 
        }
        void clear() {
            touches.clear();
            num_touches = 0;
            is_emulating = false;
        }
    };

}