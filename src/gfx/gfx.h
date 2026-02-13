#pragma once
#include "config.h"
#include "shader_utils/shader_utils.h"
#include "sprite_mesh/mesh.h"

class GFX{
    public:
    GFX(int w, int h);
    ~GFX();
    void update();
    bool window_should_close();
    unsigned int load_texture(const std::string& path);
    void draw(const unsigned int texture, float x, float y);
    private:
    GLFWwindow* window;
    SpriteMesh spritemesh;
    unsigned int shader;
    
};