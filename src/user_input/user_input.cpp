#include "user_input.h"
#include "gfx/gfx.h"



bool IsKeyPressed(int key)
{
    return (glfwGetKey(GFX::getWindow(), key) == GLFW_PRESS);
}

bool IsKeyDown(int key)
{
    return (glfwGetKey(GFX::getWindow(), key) == GLFW_RELEASE);
}

double* getRelativeCursorPosition(){
    double* position = new double[2];
    glfwGetCursorPos(GFX::getWindow(), &position[0], &position[1]);
    return position;
}