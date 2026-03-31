#pragma once
#include "config.h"
#include "mesh_struct.h"

class SpriteMesh {
public:
    void init();
    SpriteMesh();
    void prepare_shader(unsigned int shader, unsigned int texture, float posX, float posY, float width, float height, PivotPoint pv, UVCoords uv, Color color, float rotation);
    void draw(unsigned int shader, unsigned int texture, float posX, float posY, float width, float height, PivotPoint pv, UVCoords uv, Color color);
    void draw(unsigned int shader, unsigned int texture, float posX, float posY, float width, float height, PivotPoint pv, UVCoords uv, Color color, float rotation);
    
    ~SpriteMesh();

private:
    // static ShaderLocations shadesLoc;
    unsigned int VBO, VAO, vertex_count;
};