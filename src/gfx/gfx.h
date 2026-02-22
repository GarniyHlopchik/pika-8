#pragma once
#include "config.h"
#include "shader_utils/shader_utils.h"
#include "sprite_mesh/mesh.h"
#include "json_reader/get_config.h"
#include <unordered_map>
#include <string>

struct LoadedFont {
    unsigned int texture_id;
    unsigned int width;
    unsigned int height;
    FontData data;
};

class GFX {
public:
    GFX(int w, int h, const char* title);
    ~GFX();

    bool window_should_close();
    unsigned int load_texture(const std::string& path);
    void draw(const unsigned int texture, float x, float y, float width, float height, float u1 = 0.0f, float v1 = 0.0f, float u2 = 1.0f, float v2 = 1.0f);    
    void draw_text(const std::string& text, float x, float y, const std::string& font_name = "default",  float scale = 1.0f, float space_multiplier = 0.4f);
    
    static GLFWwindow* getWindow();
    static unsigned int getShader();
    void update();

    FontData get_font_data(const std::string& name);

private:
    Config config;
    static GLFWwindow* window;
    SpriteMesh spritemesh;
    static unsigned int shader;
    
    // Replace font_texture, font_texture_width, etc. with a cache map
    std::unordered_map<std::string, LoadedFont> font_cache; 
};