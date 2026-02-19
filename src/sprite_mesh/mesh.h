#pragma once
#include "config.h"

class SpriteMesh {
    public:
    void init();
    SpriteMesh();
    void draw(unsigned int shader, unsigned int texture, float posX, float posY, float u1 = 0.0f, float v1 = 0.0f, float u2 = 1.0f, float v2 = 1.0f);
    ~SpriteMesh();

    private:
    unsigned int VBO, VAO, vertex_count;

};