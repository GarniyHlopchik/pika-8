#include "user_input.h"
#include "gfx/gfx.h"



bool IsKeyPressed(int key)
{
    return !(glfwGetKey(GFX::get_window(), key) == GLFW_PRESS);
}

bool IsKeyDown(int key)
{
    return !(glfwGetKey(GFX::get_window(), key) == GLFW_RELEASE);

}

double* getRelativeCursorPosition(){
    double* position = new double[2];
    glfwGetCursorPos(GFX::get_window(), &position[0], &position[1]);
    return position;
}