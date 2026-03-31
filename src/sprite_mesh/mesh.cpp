#include "mesh.h"

SpriteMesh::~SpriteMesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

SpriteMesh::SpriteMesh(){ }

void SpriteMesh::init(){
    float data[] = {
        -1.0f,-1.0f,   0.0f,0.0f, //bottom left
        1.0f,-1.0f,   1.0f,0.0f,  // bottom right
        -1.0, 1.0f,   0.0f, 1.0f, // top left
        1.0f, 1.0f,   1.0f, 1.0f  // top right
    };

    vertex_count = 4;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);  
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW);

    //position
    glVertexAttribPointer(0,2, GL_FLOAT,GL_FALSE,16,(void*)0);//attr number, amount of elements, type, normalize,bytes to next (4*4), byte offset
    glEnableVertexAttribArray(0);

    //uv
    glVertexAttribPointer(1,2, GL_FLOAT,GL_FALSE,16,(void*)8);
    glEnableVertexAttribArray(1);
}


ShaderLocations get_locations(unsigned int shader) {
    static std::unordered_map<unsigned int, ShaderLocations> locationCache;
    auto it = locationCache.find(shader);
    if (it != locationCache.end()) {
        return it->second;
    }

    ShaderLocations loc;
    loc.position = glGetUniformLocation(shader, "uPos");
    loc.size     = glGetUniformLocation(shader, "uSize");
    loc.rotation = glGetUniformLocation(shader, "uRotation");
    loc.pivot    = glGetUniformLocation(shader, "uPivot");
    loc.u1       = glGetUniformLocation(shader, "uU1");
    loc.v1       = glGetUniformLocation(shader, "uV1");
    loc.u2       = glGetUniformLocation(shader, "uU2");
    loc.v2       = glGetUniformLocation(shader, "uV2");
    loc.texture  = glGetUniformLocation(shader, "ourTexture");
    loc.color    = glGetUniformLocation(shader, "uColor");
    
    locationCache[shader] = loc;
    return loc;
}


void SpriteMesh::prepare_shader(unsigned int shader, unsigned int texture, float posX, float posY, float width, float height, PivotPoint pv, UVCoords uv, Color color, float rotation) {
    glUseProgram(shader);
    ShaderLocations loc = get_locations(shader);

    // Position and Transform
    glUniform2f(loc.position, posX, posY);
    glUniform2f(loc.size, width, height);
    glUniform2f(loc.pivot, (float)pv.x, (float)pv.y);
    glUniform1f(loc.rotation, rotation);

    // UVs
    glUniform1f(loc.u1, uv.u1);
    glUniform1f(loc.v1, uv.v1);
    glUniform1f(loc.u2, uv.u2);
    glUniform1f(loc.v2, uv.v2);

    // Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(loc.texture, 0);

    // Color
    glUniform4f(loc.color, color.r, color.g, color.b, color.a);
}

// Draw without rotation
void SpriteMesh::draw(unsigned int shader, unsigned int texture, float posX, float posY, float width, float height, PivotPoint pv, UVCoords uv, Color color) {
    prepare_shader(shader, texture, posX, posY, width, height, pv, uv, color, 0.0f);
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertex_count);
}

// Draw with rotation
void SpriteMesh::draw(unsigned int shader, unsigned int texture, float posX, float posY, float width, float height, PivotPoint pv, UVCoords uv, Color color, float rotation) {
    // std::cout << texture << "'s rotation: " << rotation << std::endl;
    
    prepare_shader(shader, texture, posX, posY, width, height, pv, uv, color, rotation);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertex_count);
}