#include "gfx.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

GFX::GFX(int w, int h){
    if(!glfwInit()){
        std::cout << "GLFW couldn't start" << std::endl;
        
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(w,h,"PIKA-8",NULL,NULL);
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        glfwDestroyWindow(window);
        glfwTerminate();
        
    }
    glfwGetFramebufferSize(window, &w,&h);
    glViewport(0,0,w,h);

    glClearColor(0.0f,1.0f,0.0f,1.0f);
    shader = make_shader(
        "../src/shaders/vertex.txt",
        "../src/shaders/fragment.txt"
    );

    

}

GFX::~GFX(){
    glDeleteProgram(shader);
    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* GFX::getWindow(){
    return (GLFWwindow*)window;
}

bool GFX::window_should_close(){
    return glfwWindowShouldClose(window);
}

void GFX::update(){
    glfwPollEvents();
    glUseProgram(shader);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
}



