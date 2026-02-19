#include "gfx.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

GLFWwindow* GFX::window = nullptr;
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    
}  
void send_projection(int width, int height, unsigned int shader){
    glUseProgram(shader);
    unsigned int location = glGetUniformLocation(shader,"uProjection");
    float proj[16] = {
    2.0f/width,     0,        0, -1,
    0,        -2.0f/height,  0,  1,
    0,             0,        -1,  0,
    0,             0,         0,  1
    };

    glUniformMatrix4fv(location, 1, GL_TRUE, proj);

}
GFX::GFX(int w, int h, const char* title){
    if(!glfwInit()){
        std::cout << "GLFW couldn't start" << std::endl;
        
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(w,h, title,NULL,NULL);
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        glfwDestroyWindow(window);
        glfwTerminate();
        shader = make_shader();
        std::cout << "GLAD couldn't start" << std::endl;
    }
    glfwGetFramebufferSize(window, &w,&h);
    glViewport(0,0,w,h);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.11f,0.79f,0.88f,1.0f);
    shader = make_shader(    );
    send_projection(w,h,shader);

    spritemesh.init();
}

GFX::~GFX(){
    glDeleteProgram(shader);
    glfwDestroyWindow(window);
    glfwTerminate();
}

GLFWwindow* GFX::getWindow(){
    return GFX::window;
}

bool GFX::window_should_close(){
    return glfwWindowShouldClose(window);
}

void GFX::update(){
    glfwPollEvents();
    glfwSwapBuffers(window);
}




unsigned int GFX::load_texture(const std::string& path){
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if(!data){
        return 0;
    }
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    // set nearest-neighbor filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // optionally, set wrap mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return texture;
}
void GFX::draw(const unsigned int texture, float x, float y){
    spritemesh.draw(shader,texture,x,y);
}