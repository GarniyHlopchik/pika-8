#include "mesh.h"

SpriteMesh::~SpriteMesh() {
    // clean up OpenGL buffers, etc.
}
SpriteMesh::SpriteMesh(){}
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

void SpriteMesh::draw(unsigned int shader, unsigned int texture, float posX, float posY, float width, float height, UVCoords uv, Color color) {
    glUseProgram(shader);
    
    GLint posLoc = glGetUniformLocation(shader, "uPos");
    glUniform2f(posLoc, posX, posY);
    
    // Pass the actual character width/height we want to the shader!
    GLint sizeLoc = glGetUniformLocation(shader, "uSize");
    glUniform2f(sizeLoc, width, height);

    GLint uU1Loc = glGetUniformLocation(shader, "uU1");
    glUniform1f(uU1Loc, uv.u1);
    GLint uV1Loc = glGetUniformLocation(shader, "uV1");
    glUniform1f(uV1Loc, uv.v1);
    GLint uU2Loc = glGetUniformLocation(shader, "uU2");
    glUniform1f(uU2Loc, uv.u2);
    GLint uV2Loc = glGetUniformLocation(shader, "uV2");
    glUniform1f(uV2Loc, uv.v2);
    
    glActiveTexture(GL_TEXTURE0);  
    glBindTexture(GL_TEXTURE_2D, texture);

    unsigned int location = glGetUniformLocation(shader, "ourTexture");
    glUniform1i(location, 0); 

    GLint colorLoc = glGetUniformLocation(shader, "uColor");
    glUniform4f(colorLoc, color.r, color.g, color.b, color.a);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertex_count);
}