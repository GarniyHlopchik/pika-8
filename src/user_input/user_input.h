#pragma once
#include "config.h"
#include "key_codes.h"

struct InputState {
    bool keys[SDL_SCANCODE_COUNT] = {false};
    bool previous_keys[SDL_SCANCODE_COUNT] = {false};
    bool mouseButtons[8] = {false};
    float mouseX, mouseY;
};

bool IsKeyPressed(InputState state, int key);
bool IsKeyDown(InputState state, int key);
double* getRelativeCursorPosition(InputState state);