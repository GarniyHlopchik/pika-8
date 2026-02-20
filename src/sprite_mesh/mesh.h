#pragma once
#include "config.h"

class SpriteMesh {
    public:
    void init();
    SpriteMesh();
    void draw(unsigned int shader, unsigned int texture, float posX, float posY, float width, float height, float u1, float v1, float u2, float v2);
    ~SpriteMesh();

    private:
    unsigned int VBO, VAO, vertex_count;

};