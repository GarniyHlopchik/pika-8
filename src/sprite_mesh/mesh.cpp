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



void SpriteMesh::prepare_shader(unsigned int shader, unsigned int texture, float posX, float posY, float width, float height, PivotPoint pv, UVCoords uv, Color color, float rotation) {
    glUseProgram(shader);

    // Position and Size
    glUniform2f(glGetUniformLocation(shader, "uPos"), posX, posY);
    glUniform2f(glGetUniformLocation(shader, "uSize"), width, height);
    
    // Rotation (Defaults to 0.0f in the non-rotating overload)
    glUniform1f(glGetUniformLocation(shader, "uRotation"), rotation);

    // UV Coordinates
    glUniform1f(glGetUniformLocation(shader, "uU1"), uv.u1);
    glUniform1f(glGetUniformLocation(shader, "uV1"), uv.v1);
    glUniform1f(glGetUniformLocation(shader, "uU2"), uv.u2);
    glUniform1f(glGetUniformLocation(shader, "uV2"), uv.v2);

    // Texture Binding
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader, "ourTexture"), 0);

    // Color
    glUniform4f(glGetUniformLocation(shader, "uColor"), color.r, color.g, color.b, color.a);

    // Pivot
    glUniform2f(glGetUniformLocation(shader, "uPivot"), pv.x, pv.y);
    // std::cout << "pivot is: { " << pv.x << ", " << pv.y << " }" << std::endl;

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