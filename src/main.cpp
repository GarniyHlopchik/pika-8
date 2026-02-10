#include "config.h"
#include "sprite_mesh/mesh.h"
#include "shader_utils/shader_utils.h"
lua_State* L;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void log_error(const char* err){
    std::cout << err << std::endl;
}
void call_lua(const char* name) {
    lua_getglobal(L, name);
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        log_error(err);
        lua_pop(L, 1);
    }
}
void call_lua_update(float delta) {
    lua_getglobal(L, "_update");

    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }

    // Push arguments (in order)
    lua_pushnumber(L, delta);

    // Call: 1 argument, 0 return values
    if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        log_error(err);
        lua_pop(L, 1);
    }
}
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
    L = luaL_newstate();  // Lua vm
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
    call_lua("_init"); // call init callback in lua
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
        call_lua_update(dt);

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
