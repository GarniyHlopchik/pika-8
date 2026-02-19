#include "shader_utils.h"

// Define shaders
const char* fragSource =
    "#version 330 core\n"
    "in vec2 vUV;\n"
    "out vec4 screenColor;\n"
    "uniform sampler2D ourTexture;\n"
    "void main()\n"
    "{\n"
    "        screenColor = texture(ourTexture, vUV);\n"
    "}\n";

const char* vertexSource =
    "#version 330 core \n"
    "layout (location = 0) in vec2 vertexPos; \n"
    "layout (location = 1) in vec2 vertexUV; \n"
    "uniform vec2 uPos;\n"
    "uniform vec2 uSize;\n"
    "uniform mat4 uProjection;\n"
    "uniform float uU1, uV1, uU2, uV2;\n"
    "out vec2 vUV;\n"
    "void main() \n"
    "{\n"
    "   vec2 worldPos = uPos + vertexPos * uSize;  \n"
    "   gl_Position = uProjection * vec4(worldPos, 0.0, 1.0);  \n"
    "   vUV = vec2(mix(uU1, uU2, vertexUV.x), mix(uV1, uV2, vertexUV.y));  \n"
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