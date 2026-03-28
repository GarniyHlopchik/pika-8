#pragma once
#include "config.h"
#include "shader_utils/shader_utils.h"
#include "sprite_mesh/mesh.h"
#include "json_reader/get_config.h"
#include <unordered_map>
#include <string>
#include "gfx_struct.h"
#include "user_input/user_input.h"
#include "file_resolve/file_system.h"
#include <algorithm>
#include "../mobile_input/input_state.h"

class GFX {
public:
    GFX(int w, int h, const char* title, InputState &p_state);
    ~GFX();

    unsigned int load_texture(const std::string& path);
    
    // No color nor rotation specification
    void draw(const unsigned int texture, float x, float y, float width, float height, PivotPoint pv, UVCoords uv);
    // Color specification, but not rotation
    void draw(const unsigned int texture, float x, float y, float width, float height, PivotPoint pv, UVCoords uv, Color color);
    // Color and rotation
    void draw(const unsigned int texture, float x, float y, float width, float height, PivotPoint pv, UVCoords uv, Color color, float rotation);
    // No pivot point, but color
    void draw(const unsigned int texture, float x, float y, float width, float height, UVCoords uv, Color color);


    FontData get_font_data(const std::string& name);

    static SDL_Window* get_window();
    std::tuple<int,int> get_screen_size();
    static unsigned int get_shader();
    bool is_running();
    void update();
    void resize(int width, int height);
    static std::vector<int> get_image_dimensions(const std::string& path);
    static std::vector<int> get_image_dimensions(const unsigned int id);

    static std::string get_texture_path(const unsigned int id);
    static void add_new_image(const LoadedImages img);
    void close();
    mobile_input::InputState* getMobileInputState() { return &mobile_input_state; }
private:
    Config config;
    static SDL_Window* window;
    static SDL_GLContext gl_context;
    static bool running;
    SpriteMesh spritemesh;
    static unsigned int shader;
    InputState &input_state;
    mobile_input::InputState mobile_input_state;
    void handleTouch(const SDL_Event& event);
    void handleMouse(const SDL_Event& event);
};