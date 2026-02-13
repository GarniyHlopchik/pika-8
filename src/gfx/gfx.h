#pragma once
#include "config.h"
#include "shader_utils/shader_utils.h"

class GFX{
    public:
    GFX(int w, int h);
    ~GFX();

    bool window_should_close();
    static GLFWwindow* getWindow();
    void update();

    private:
    GLFWwindow* window;
    unsigned int shader;
};