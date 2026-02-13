#include "config.h"
#include "sprite_mesh/mesh.h"

unsigned int make_module(const std::string& filepath, unsigned int module_type);
unsigned int make_shader(const std::string& vertex_filepath, const std::string& fragment_filepath);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
int main(){
    //opengl setup------------------------
    GLFWwindow* window;

    if(!glfwInit()){
        std::cout << "GLFW couldn't start" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    const char* title = "PIKA-8";
    const int width = 512;
    const int height = 512;
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    int w,h;
    glfwGetFramebufferSize(window, &w,&h);
    glViewport(0,0,w,h);
    //---------------------------------------------------
    lua_State* L = luaL_newstate();  // Lua vm
    luaL_openlibs(L);               // setup
    luaL_dostring(L, "print('hello from lua')"); //just test but it's so beautiful I'm leaving it


    glClearColor(0.0f,1.0f,0.0f,1.0f);
    unsigned int shader = make_shader(
        "../src/shaders/vertex.txt",
        "../src/shaders/fragment.txt"
    );
    //draw sprite obj
    SpriteMesh* sprite = new SpriteMesh();



    while(!glfwWindowShouldClose(window)){
        
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader);
        sprite->draw();
        glfwSwapBuffers(window);

    }



    glDeleteProgram(shader);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  
unsigned int make_shader(const std::string& vertex_filepath, const std::string& fragment_filepath){
    std::vector<unsigned> modules;
    modules.push_back(make_module(vertex_filepath, GL_VERTEX_SHADER));
    modules.push_back(make_module(fragment_filepath, GL_FRAGMENT_SHADER));

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
unsigned int make_module(const std::string& filepath, unsigned int module_type){
    std::ifstream file;
    std::stringstream bufferedLines;
    std::string line;

    file.open(filepath);
    while(std::getline(file,line)){
        bufferedLines << line << "\n";
    }
    std::string ShaderSource = bufferedLines.str();
    const char* ShaderSrc = ShaderSource.c_str();
    bufferedLines.str("");
    file.close();

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