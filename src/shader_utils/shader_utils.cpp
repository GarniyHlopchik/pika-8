#include "shader_utils.h"

// Define shaders
const char* vertexSource = R"(
layout (location = 0) in vec2 vertexPos; 
layout (location = 1) in vec2 vertexUV; 
layout (location = 2) in vec4 aColor;

uniform mat4 uProjection;

out vec2 vUV;
out vec4 vColor;

void main()
{
    gl_Position = uProjection * vec4(vertexPos, 0.0, 1.0);  
    vUV = vertexUV;
    vColor = aColor;
}
)";

const char* fragSource = R"(
in vec2 vUV;
in vec4 vColor;
out vec4 screenColor;

uniform sampler2D ourTexture;

void main() {
    screenColor = texture(ourTexture, vUV) * vColor;
}
)";

const char* debugVertexSource = R"(
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec4 aColor;

uniform mat4 uProjection;

out vec4 vColor;

void main()
{
    gl_Position = uProjection * vec4(vertexPos, 1.0);
    vColor = aColor;
}
)";

const char* debugFragSource = R"(
in vec4 vColor;
out vec4 screenColor;

void main()
{
    screenColor = vColor;
}
)";


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

unsigned int make_debug_shader(){
    std::vector<unsigned> modules;
    modules.push_back(make_debug_module(GL_VERTEX_SHADER));
    modules.push_back(make_debug_module(GL_FRAGMENT_SHADER));

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
        std::cout << "Debug shader linking error:\n" << errLog << std::endl;
    }
    for(unsigned int shaderModule : modules){
        glDeleteShader(shaderModule);
    }
    return shader;
}

unsigned int make_debug_module(unsigned int module_type){
    std::string header = "";
    #ifdef PIKA_GLES
        header = "#version 300 es\nprecision mediump float;\n";
    #else
        header = "#version 330 core\n";
    #endif

    std::string fullSource = header + (module_type == GL_VERTEX_SHADER ? debugVertexSource : debugFragSource);
    const char* ShaderSrc = fullSource.c_str();

    unsigned int shaderModule = glCreateShader(module_type);
    glShaderSource(shaderModule, 1, &ShaderSrc, nullptr);
    glCompileShader(shaderModule);

    int success;
    glGetShaderiv(shaderModule, GL_COMPILE_STATUS, &success);
    if(!success){
        char errLog[1024];
        glGetShaderInfoLog(shaderModule, 1024,NULL,errLog);
        std::cout << "Debug shader module compilation error:\n" << errLog << std::endl;
    }
    return shaderModule;
}

unsigned int make_module(unsigned int module_type){
    std::string header = "";
    #ifdef PIKA_GLES
        header = "#version 300 es\nprecision mediump float;\n";
    #else
        header = "#version 330 core\n";
    #endif
    std::string fullFrag = header+fragSource;
    std::string fullVertex = header+vertexSource;
    const char* ShaderSrc = (module_type == GL_VERTEX_SHADER) ? fullVertex.c_str() : fullFrag.c_str();

    unsigned int shaderModule = glCreateShader(module_type);
    glShaderSource(shaderModule, 1, &ShaderSrc, nullptr);
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