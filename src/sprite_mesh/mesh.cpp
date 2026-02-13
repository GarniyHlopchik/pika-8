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
void SpriteMesh::draw(unsigned int shader, unsigned int texture, int posX, int posY){
    glUseProgram(shader);
    GLint posLoc = glGetUniformLocation(shader, "uPos");
    glUniform2f(posLoc, posX, posY);
    
    

   
    
    glActiveTexture(GL_TEXTURE0);  // use unit 0
    glBindTexture(GL_TEXTURE_2D, texture);
    int width, height;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height); 

    GLint sizeLoc = glGetUniformLocation(shader, "uSize");
    glUniform2f(sizeLoc, width, height);

    unsigned int location = glGetUniformLocation(shader, "ourTexture");
    glUniform1i(location, 0); // 0 = GL_TEXTURE0

     glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP,0,vertex_count);
}