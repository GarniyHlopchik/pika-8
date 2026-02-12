#pragma once
#include "config.h"
#include "shader_utils/shader_utils.h"

class GFX{
    public:
    GFX(int w, int h);
    ~GFX();
    void update();
    bool window_should_close();
    private:
    GLFWwindow* window;
    unsigned int shader;
};