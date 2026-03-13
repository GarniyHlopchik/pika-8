#pragma once
#include "config.h"
#include "mesh_struct.h"

class SpriteMesh {
    public:
    void init();
    SpriteMesh();
    void draw(unsigned int shader, unsigned int texture, float posX, float posY, float width, float height, UVCoords uv, Color color);
    ~SpriteMesh();

    private:
    unsigned int VBO, VAO, vertex_count;

};