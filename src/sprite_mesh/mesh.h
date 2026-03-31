#pragma once
#include "config.h"
#include "mesh_struct.h"

class SpriteMesh {
public:
    void init();
    SpriteMesh();
    void draw(
        unsigned int shader, unsigned int texture, float posX, float posY, 
        float width, float height, PivotPoint pv, UVCoords uv, Color color);

    void draw(
        unsigned int shader, unsigned int texture, float posX, float posY, 
        float width, float height, PivotPoint pv, UVCoords uv, Color color, 
        float rotation);

    void flush(unsigned int shader);
    
    ~SpriteMesh();

private:
    static const unsigned int MAX_BATCH_SPRITES = 4096;
    unsigned int VBO, VAO, EBO;
    unsigned int batchTexture = 0;
    bool batchTextureActive = false;
    std::vector<SpriteVertexShader> batchVertices;
    std::vector<SpriteDrawCommand> textureSortedVertexShader;

    void add_sprite_to_batch(
        unsigned int shader, unsigned int texture, 
        float posX, float posY, float width, float height, const PivotPoint& pv, 
        const UVCoords& uv, const Color& color, float rotation);

};