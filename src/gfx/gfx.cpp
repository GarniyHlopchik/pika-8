#include "gfx.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

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
    send_projection(w,h);

    spritemesh.init();
}

GFX::~GFX(){
    glDeleteProgram(shader);
    for (auto const& pair : font_cache) {
        // pair.second gets the LoadedFont struct
        unsigned int tex = pair.second.texture_id; 
        glDeleteTextures(1, &tex);
    }    
    glfwDestroyWindow(window);
    glfwTerminate();
    for (auto const& [name, font] : font_cache) {
        glDeleteTextures(1, &font.texture_id);
    }
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



void GFX::draw(const unsigned int texture, float x, float y, float width, float height, UVCoords uv) {
    spritemesh.draw(shader, texture, x, y, width, height, (float)uv.u1, (float)uv.v1, (float)uv.u2, (float)uv.v2);
}


FontData GFX::get_font_data(const std::string& name) {
    for (const auto& font : config.get_fonts()) {
        if (font.name == name) {
            return font;
        }
    }
    throw std::runtime_error("Font not found: " + name);
}

bool GFX::is_font_loaded(const std::string& font_name){
    return (font_cache.find(font_name) != font_cache.end());
}

void GFX::load_font(const std::string& font_name){
    FontData font_data = get_font_data(font_name);    
    unsigned int tex_id = load_texture(font_data.path);
    
    if (!tex_id) return;

    unsigned int img_width = 0, img_height = 0;
    std::vector<int> dims = get_image_dimensions(font_data.path);
    img_width = dims[0];
    img_height = dims[1];

    font_cache[font_name] = {tex_id, img_width, img_height, font_data};
}

size_t GFX::get_char_index(char c, const std::string& charset) const {
    size_t index = charset.find(c);
    
    // Fallback to uppercase if lowercase isn't found
    if (index == std::string::npos) {
        char upper_c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
        index = charset.find(upper_c);
    }
    
    return index;
}

UVCoords GFX::calculate_uv_coords(size_t index, const LoadedFont& font) const {
    const int chars_per_row = font.width / font.data.char_width;
    const int total_rows = font.height / font.data.char_height;

    int col = index % chars_per_row;
    int row = index / chars_per_row;
    
    // Note: Are you sure these shouldn't be font.width / font.height?
    /// Should be exactly that. No clue how it worked....
    const float tex_width = font.width; 
    const float tex_height = font.height;

    const float margin_u = 0.1f / tex_width;
    const float margin_v = 0.1f / tex_height;
        
    return {
        (static_cast<float>(col) / chars_per_row) + margin_u,
        (static_cast<float>(row) / total_rows) + margin_v,
        (static_cast<float>(col + 1) / chars_per_row) - margin_u,
        (static_cast<float>(row + 1) / total_rows) - margin_v
    };
}

void GFX::draw_text(const std::string& text, float x, float y, const std::string& font_name, float scale, float space_multiplier) {
    
    // load font to cash onse
    if (is_font_loaded(font_name) == false) {
        load_font(font_name);
    }
    
    if (is_font_loaded(font_name) == false) return;

    const LoadedFont& font = font_cache[font_name];

    float draw_width = font.data.char_width * scale; 
    float draw_height = font.data.char_height * scale; 

    float cursor_x = x; 

    // Drawing text by charecter
    for (char c : text) {
        if (c == ' ') {
            cursor_x += (draw_width * space_multiplier); 
            continue;
        }

        size_t index = get_char_index(c, font.data.charset);
        if (index == std::string::npos) continue;
        
        UVCoords uv = calculate_uv_coords(index, font);

        // Use the cached texture ID
        draw(font.texture_id, cursor_x, y, draw_width, draw_height, uv);

        cursor_x += draw_width; 
    }
}