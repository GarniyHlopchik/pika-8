#pragma once
#include "config.h"

class SpriteMesh {
    public:
    void init();
    SpriteMesh();
    void draw(unsigned int shader, unsigned int texture, float posX, float posY);
    ~SpriteMesh();

    private:
    unsigned int VBO, VAO, vertex_count;

};