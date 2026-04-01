#include "mesh.h"
#include "../gfx/sprite/sprite.h"
#include <climits>

SpriteMesh::~SpriteMesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

SpriteMesh::SpriteMesh(){ }

void SpriteMesh::init() {
    // Vertex Array Object: data formatter
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Vertex Buffer Object: tells GPU that the objects will be draws every frame (GL_DYNAMIC_DRAW)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_SPRITES * 4 * sizeof(SpriteVertexShader), nullptr, GL_DYNAMIC_DRAW);

    // Element Buffer Object: teaches GPU how to draw rectangle from triangle without repeating corners
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    std::vector<unsigned int> indices;
    indices.reserve(MAX_BATCH_SPRITES * 6);
    for (unsigned int i = 0; i < MAX_BATCH_SPRITES; ++i) {
        unsigned int base = i * 4;
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    constexpr size_t stride = sizeof(SpriteVertexShader);
    
    /// tells GPU where to all the shit from SpriteVertexShader struct.
    /// and how nuch to skip for the next item needed [ (void*)(sizeof( {TYPE} ) * N) ]
    
    // location 0 - position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)(0)); 

    // location 1 - uv
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 2)); 

    // location 2 - color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 4));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    batchVertices.reserve(MAX_BATCH_SPRITES * 4);
    textureSortedVertexShader.reserve(MAX_BATCH_SPRITES);
}

float degreesToRadians(float degrees) {
    const float pi = std::acos(-1.0);
    return degrees * (pi / 180.0);
}

void SpriteMesh::add_sprite_to_batch(
    unsigned int shader, unsigned int texture, 
    float posX, float posY, float width, float height, const PivotPoint& pv, 
    const UVCoords& uv, const Color& color, float rotation,
    unsigned int sortIndex
) {
    // Precalculate everything needed befor flushing
    // calculate trigonometry for rotation
    const float rads = degreesToRadians(rotation);
    const float cosR = cos(rads);
    const float sinR = sin(rads);

    // pivon { 0, 0 } is the center
    const float dx = -(width * 0.5f) - (width * pv.x); 
    const float dy = -(height * 0.5f) - (height * pv.y);

    const float localX[4] = { dx, dx + width, dx + width, dx };
    const float localY[4] = { dy, dy, dy + height, dy + height };

    const float uvX[4] = { uv.u1, uv.u2, uv.u2, uv.u1 };
    const float uvY[4] = { uv.v1, uv.v1, uv.v2, uv.v2 };

    SpriteDrawCommand spriteDrawCommand;
    spriteDrawCommand.texture = texture;
    spriteDrawCommand.index = (sortIndex == UINT_MAX) ? batchSequenceIndex++ : sortIndex;
    
    // Apply rotation and translation to all 4 corners
    for (int i = 0; i < 4; ++i) {
        spriteDrawCommand.vertices[i] = {
            posX + (localX[i] * cosR - localY[i] * sinR), // x
            posY + (localX[i] * sinR + localY[i] * cosR), // y
            uvX[i],                                       // u
            uvY[i],                                       // v
            color.r, color.g, color.b, color.a            // r, g, b, a
        };
    }
    textureSortedVertexShader.emplace_back(spriteDrawCommand);

}

void SpriteMesh::draw(unsigned int shader, const Sprite& sprite) {
    add_sprite_to_batch(shader, sprite);
}

void SpriteMesh::add_sprite_to_batch(unsigned int shader, const Sprite& sprite) {
    add_sprite_to_batch(
        shader,
        sprite.get_texture(),
        sprite.get_x(),
        sprite.get_y(),
        sprite.get_width(),
        sprite.get_height(),
        sprite.get_pivot(),
        sprite.get_uv(),
        sprite.get_color(),
        static_cast<float>(sprite.get_rotation()),
        sprite.get_sort_index()
    );
}

void SpriteMesh::flush(unsigned int shader) {
    batchSequenceIndex = 0;
    if (textureSortedVertexShader.empty()) {
        return;
    }

    std::sort(textureSortedVertexShader.begin(), textureSortedVertexShader.end(), 
        [](const SpriteDrawCommand& a, const SpriteDrawCommand& b) {
            if (a.index != b.index) {
                return a.index < b.index; 
            }
            return a.texture < b.texture; 
        }
    ); 

    
    glUseProgram(shader);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // initial state for the loop
    batchVertices.clear();
    unsigned int currentTexture = textureSortedVertexShader[0].texture;

    // bind the first texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, currentTexture);
    glUniform1i(glGetUniformLocation(shader, "ourTexture"), 0);


    for (const auto& spriteCommand : textureSortedVertexShader) {
        
        // check if the texture changed or we are out of space
        if (spriteCommand.texture != currentTexture || (batchVertices.size() / 4) >= MAX_BATCH_SPRITES) {
            
            // draw what we have so far
            glBufferSubData(GL_ARRAY_BUFFER, 0, batchVertices.size() * sizeof(SpriteVertexShader), batchVertices.data());
            unsigned int indexCount = static_cast<unsigned int>(batchVertices.size() / 4) * 6;
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

            // clear the queue
            batchVertices.clear();
            currentTexture = spriteCommand.texture;
            
            // Bind the new texture for the next batch
            glBindTexture(GL_TEXTURE_2D, currentTexture);
        }

        // place the drawing commands onto the train
        for (int i = 0; i < 4; ++i) {
            batchVertices.emplace_back(spriteCommand.vertices[i]);
        }
    }

    // we reached the end of the list. Draw whatever is left
    if (!batchVertices.empty()) {
        glBufferSubData(GL_ARRAY_BUFFER, 0, batchVertices.size() * sizeof(SpriteVertexShader), batchVertices.data());
        unsigned int indexCount = static_cast<unsigned int>(batchVertices.size() / 4) * 6;
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }

    // clean up for the next frame
    textureSortedVertexShader.clear();
    batchVertices.clear();
}



void SpriteMesh::draw(unsigned int shader, unsigned int texture, float posX, float posY, float width, float height, PivotPoint pv, UVCoords uv, Color color) {
    add_sprite_to_batch(shader, texture, posX, posY, width, height, pv, uv, color, 0.0f);
}

// Draw with rotation
void SpriteMesh::draw(unsigned int shader, unsigned int texture, float posX, float posY, float width, float height, PivotPoint pv, UVCoords uv, Color color, float rotation) {
    add_sprite_to_batch(shader, texture, posX, posY, width, height, pv, uv, color, rotation);
}


