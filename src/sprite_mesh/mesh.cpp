#include "mesh.h"

SpriteMesh::SpriteMesh(){
    float data[] = {
       -1.0f, -1.0f, 0.0f,   1.0f, 1.0f, 1.0f, //bottom left
        1.0f, -1.0f, 0.0f,   1.0f, 1.0f, 1.0f,  // bottom right
       -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 1.0f, // top left
        1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 0.0f  // top right
    };

    vertex_count = 4;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);  
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW);

    //position
    glVertexAttribPointer(0,3, GL_FLOAT,GL_FALSE,24,(void*)0);//attr number, amount of elements, type, normalize,bytes to next (6*4), byte offset
    glEnableVertexAttribArray(0);

    //color
    glVertexAttribPointer(1,3, GL_FLOAT,GL_FALSE,24,(void*)12);
    glEnableVertexAttribArray(1);
}

void SpriteMesh::draw(){
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP,0,vertex_count);
}