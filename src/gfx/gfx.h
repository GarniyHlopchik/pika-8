#pragma once
#include "config.h"
#include "shader_utils/shader_utils.h"
#include "sprite_mesh/mesh.h"
#include "json_reader/get_config.h"
#include <unordered_map>
#include <string>
#include "gfx_struct.h"


class GFX {
public:
    GFX(int w, int h, const char* title);
    ~GFX();

    bool window_should_close();
    unsigned int load_texture(const std::string& path);
    void draw(const unsigned int texture, float x, float y, float width, float height, UVCoords uv);    
    void draw(const unsigned int texture, float x, float y, float width, float height, UVCoords uv, Color color);

    FontData get_font_data(const std::string& name);

    static GLFWwindow* get_window();
    std::tuple<int,int> get_screen_size();
    static unsigned int get_shader();
    void update();

    static std::vector<int> get_image_dimensions(const std::string& path);
    static std::vector<int> get_image_dimensions(const unsigned int id);

    static std::string get_texture_path(const unsigned int id);
    static void add_new_image(const LoadedImages img);

private:
    Config config;
    static GLFWwindow* window;
    SpriteMesh spritemesh;
    static unsigned int shader;
};