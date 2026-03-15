#pragma once

#include <vector>
//#include <SDL2/SDL.h>
typedef unsigned int Uint32;
namespace mobile_input {  

    struct TouchPoint {
        int id;              
        float x, y;          
        float dx, dy;         
        bool down;     
        bool just_pressed;   
        bool just_released;
        
        Uint32 timestamp;     
    };

    struct InputState {
        
        std::vector<TouchPoint> touches;

        
        int num_touches = 0;

        struct {
            bool active = false;
            int touch_id = -1;        
            float x, y;               
        } mouse_emulation;

        void resetFrame() {
            for (auto& touch : touches) {
                touch.just_pressed = false;
                touch.just_released = false;
                touch.dx = 0;
                touch.dy = 0;
            }
        }

        void clear() {
            touches.clear();
            num_touches = 0;
            mouse_emulation.active = false;
        }
    };

} 