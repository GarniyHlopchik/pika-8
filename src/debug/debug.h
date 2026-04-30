#pragma once
#include "gfx/gfx.h"
#include "sprite_mesh/mesh.h"
#include <glad/glad.h>
#include "debug_struct.h"

class Debug {
public:
    Debug();
    ~Debug();

    static Debug& get_instance();
    static void init(unsigned int shader);
    static void set_projection(int width, int height);
    static void flush(unsigned int shader);
    static void begin_frame();
    static void end_frame();

    static void draw_line(float x1, float y1, float x2, float y2, Color color);
    static void draw_rect(float x, float y, float width, float height, Color color);
    static void draw_circle(float centerX, float centerY, float radius, Color color);
    static void draw_polygon(const std::vector<std::pair<float, float>>& vertices, Color color);

private:
    static void draw_debug_shape(std::vector<DebugVertex>& vertices, DebugPrimitiveType type);
    
    unsigned int VAO, VBO;
    std::vector<DebugDrawCommand> frameCommands;
    unsigned int shader;
};