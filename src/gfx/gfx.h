#pragma once
#include "config.h"
#include "shader_utils/shader_utils.h"
#include "sprite_mesh/mesh.h"

class GFX{
    public:
    GFX(int w, int h, const char* title);
    ~GFX();

    bool window_should_close();
    unsigned int load_texture(const std::string& path);
    void draw(const unsigned int texture, float x, float y, float width, float height, float u1 = 0.0f, float v1 = 0.0f, float u2 = 1.0f, float v2 = 1.0f);    
    void draw_text(const std::string& text, float x, float y, float scale = 1.0f, float space_multiplier = 4.0f);
    static GLFWwindow* getWindow();
    static unsigned int getShader();
    void update();

    private:
    static GLFWwindow* window;
    SpriteMesh spritemesh;
    static unsigned int shader;
    unsigned int font_texture;
    int font_texture_width;
    int font_texture_height;
    
};