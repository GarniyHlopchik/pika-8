#pragma once
#include "config.h"
#include "key_codes.h"
#include "../mobile_input/input_state.h"  
#include <SDL3/SDL_events.h>     

struct InputState {
    bool keys[SDL_SCANCODE_COUNT] = {false};
    bool previous_keys[SDL_SCANCODE_COUNT] = {false};
    bool mouseButtons[8] = {false};
    float mouseX, mouseY;
};

bool IsKeyPressed(InputState state, int key);
bool IsKeyDown(InputState state, int key);
double* getRelativeCursorPosition(InputState state);

void initTouchInput();                          
void setTouchWindowSize(int width, int height); 
void resetTouchFrame();                         
mobile_input::InputState* getTouchState();      

void handleTouchEvent(const SDL_Event& event);
void handleMouseEvent(const SDL_Event& event);