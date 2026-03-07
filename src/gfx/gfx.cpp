#include "gfx.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include "file_resolve/file_system.h"

std::vector<LoadedImages> loaded_images;
GLFWwindow* GFX::window = nullptr;
unsigned int GFX::shader = 0;
void send_projection(int width, int height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    send_projection(width,height);
    
}  
void send_projection(int width, int height){
    unsigned int shader = GFX::get_shader();
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
unsigned int GFX::get_shader(){
    return GFX::shader;
}
std::tuple<int,int> GFX::get_screen_size(){
    int width, height;
    glfwGetWindowSize(window,&width,&height);
    return std::make_tuple(width,height);
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
    //glClearColor(0.11f,0.79f,0.88f,1.0f);
    shader = make_shader(    );
    send_projection(w,h);

    spritemesh.init();
}

GFX::~GFX(){
    glDeleteProgram(shader);
    
    glfwDestroyWindow(window);
    glfwTerminate();

}

GLFWwindow* GFX::get_window() {
    return GFX::window;
}

std::vector<int> GFX::get_image_dimensions(const std::string& path) {
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if(!data){
        return {0,0};
    }
    stbi_image_free(data);
    return {width, height};
}

std::vector<int> GFX::get_image_dimensions(const unsigned int texture) {
    return get_image_dimensions(get_texture_path(texture));
}

std::string GFX::get_texture_path(const unsigned int id){
    for (const LoadedImages& img : loaded_images) {
        if (img.id == id) {
            return img.path;
        }
    }
    throw std::runtime_error("Image not found with id: " + id);
}

void GFX::add_new_image(const LoadedImages img){
    loaded_images.push_back(img);
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

    Resource res = FileSystem::get_resource(path);
    if(!res.is_valid()){
        return 0;
    }

    unsigned char *data = stbi_load_from_memory(res.data.get(),res.size, &width, &height, &nrChannels, 0);
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

FontData GFX::get_font_data(const std::string& name) {
    for (const auto& font : config.get_fonts()) {
        if (font.name == name) {
            return font;
        }
    }
    throw std::runtime_error("Font not found: " + name);
}

void GFX::draw(const unsigned int texture, float x, float y, float width, float height, UVCoords uv) {
    spritemesh.draw(shader, texture, x, y, width, height, (float)uv.u1, (float)uv.v1, (float)uv.u2, (float)uv.v2);
}
