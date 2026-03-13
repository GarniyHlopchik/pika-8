#include "user_input.h"
#include "gfx/gfx.h"


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

