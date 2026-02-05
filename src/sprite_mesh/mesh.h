#pragma once
#include "config.h"

class SpriteMesh {
    public:
    SpriteMesh();
    void draw();
    ~SpriteMesh();

    private:
    unsigned int VBO, VAO, vertex_count;

};