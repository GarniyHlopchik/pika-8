#include "config.h"
#include "sprite_mesh/mesh.h"
#
unsigned int make_module(const std::string& filepath, unsigned int module_type);
unsigned int make_shader(const std::string& vertex_filepath, const std::string& fragment_filepath);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
int l_cls(lua_State* L){
    glClear(GL_COLOR_BUFFER_BIT);
    std::cout << "Screan cleared" << std::endl;
    return 0;
}
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

    window = glfwCreateWindow(512,512,"PIKA-8",NULL,NULL);
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
    //lua setup-----------------------------
    lua_State* L = luaL_newstate();  // Lua vm
    luaL_openlibs(L);               // setup
    lua_register(L, "cls", l_cls); //registering cpp function to lua
    if (luaL_dofile(L, "main.lua") != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        std::cout << "Issue loading main.lua" << std::endl;
        std::cout << err << std::endl;
        std::cout << "\nPress Enter to exit...";
        std::cin.get();
        return -1;
    }
    //----------------------

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