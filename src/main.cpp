#include "config.h"
#include "sprite_mesh/mesh.h"
#include "shader_utils/shader_utils.h"
#include "lua_system/lua_system.h"
lua_State* L;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
int l_cls(lua_State* L){
    glClear(GL_COLOR_BUFFER_BIT);
    std::cout << "Screen cleared" << std::endl;
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
    LuaSystem lua;

    static const luaL_Reg gfx_lib[] = {
    {"cls", l_cls},
    {NULL, NULL}
    };
    lua.bind_lib(gfx_lib,"GFX");

    lua.load_script("main.lua");
    lua.call("_init");
    //----------------------

    glClearColor(0.0f,1.0f,0.0f,1.0f);
    unsigned int shader = make_shader(
        "../src/shaders/vertex.txt",
        "../src/shaders/fragment.txt"
    );
    //draw sprite obj
    SpriteMesh* sprite = new SpriteMesh();


    //update loop
    auto last_time = std::chrono::high_resolution_clock::now();
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();

        //count delta
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> delta = now - last_time;
        last_time = now;
        float dt = delta.count();
        lua.call_update(dt);

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
