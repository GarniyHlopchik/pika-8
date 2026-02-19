#include "shader_utils.h"

// Define shaders
const char* vertexSource =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "layout(location = 1) in vec3 aColor;\n"
    "layout(location = 2) in vec2 aTexCoord;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos, 1.0);\n"
    "    TexCoord = aTexCoord;\n"
    "}\n";

const char* fragSource =
    "#version 330 core \n"
    "out vec4 FragColor; \n"
    "in vec3 ourColor; \n"
    "in vec2 TexCoord; \n"
    "uniform sampler2D ourTexture; \n"
    "void main() \n"
    "{\n"
    "  FragColor = texture(ourTexture, TexCoord);\n"
    "}\n";

unsigned int make_shader(){
    std::vector<unsigned> modules;
    modules.push_back(make_module(GL_VERTEX_SHADER));
    modules.push_back(make_module(GL_FRAGMENT_SHADER));

    unsigned int shader = glCreateProgram();
    for(unsigned int shaderModule : modules){
        glAttachShader(shader, shaderModule);
    }
    glLinkProgram(shader);
    int success;
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if(!success){
        char errLog[1024];
        glGetProgramInfoLog(shader, 1024,NULL,errLog);
        std::cout << "Shader linking error:\n" << errLog << std::endl;

    }
    for(unsigned int shaderModule : modules){
        glDeleteShader(shaderModule);
    }
    return shader;
}

unsigned int make_module(unsigned int module_type){
    const char* ShaderSrc = (module_type == GL_VERTEX_SHADER) ? vertexSource : fragSource;

    unsigned int shaderModule = glCreateShader(module_type);
    glShaderSource(shaderModule,1,&ShaderSrc,NULL);
    glCompileShader(shaderModule);

    int success;
    glGetShaderiv(shaderModule, GL_COMPILE_STATUS, &success);
    if(!success){
        char errLog[1024];
        glGetShaderInfoLog(shaderModule, 1024,NULL,errLog);
        std::cout << "Shader module compilation error:\n" << errLog << std::endl;

    }

    return shaderModule;
}