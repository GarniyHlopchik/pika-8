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

struct LoadedImages
{
    std::string path;
    unsigned int id;
};

struct Color
{
    int r, g, b, a;
};

struct UVCoords {
    float u1, v1, u2, v2;
};

class GFX {
public:
    GFX(int w, int h, const char* title);
    ~GFX();

    bool window_should_close();
    unsigned int load_texture(const std::string& path);
    void draw(const unsigned int texture, float x, float y, float width, float height, UVCoords uv);    
    void draw_text(const std::string& text, float x, float y, const std::string& font_name = "default",  float scale = 1.0f, float space_multiplier = 0.4f);
    
    UVCoords calculate_uv_coords(size_t index, const LoadedFont& font) const;
    size_t get_char_index(char c, const std::string& charset) const;
    void load_font(const std::string& font_name);
    bool is_font_loaded(const std::string& font_name);

    static GLFWwindow* get_window();
    static unsigned int get_shader();
    void update();

    FontData get_font_data(const std::string& name);
    
    static std::vector<int> get_image_dimensions(const std::string& path);
    static std::vector<int> get_image_dimensions(const unsigned int id);

    static std::string get_texture_path(const unsigned int id);
    static void add_new_image(const LoadedImages img);

private:
    Config config;
    static GLFWwindow* window;
    SpriteMesh spritemesh;
    static unsigned int shader;
    
    std::unordered_map<std::string, LoadedFont> font_cache; 
};