#include "gfx.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"


GLFWwindow* GFX::window = nullptr;
unsigned int GFX::shader = 0;
void send_projection(int width, int height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    send_projection(width,height);
    
}  
void send_projection(int width, int height){
    unsigned int shader = GFX::getShader();
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
unsigned int GFX::getShader(){
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
    if(font_texture) glDeleteTextures(1, &font_texture);
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

void GFX::draw(const unsigned int texture, float x, float y, float width, float height, float u1, float v1, float u2, float v2) {
    spritemesh.draw(shader, texture, x, y, width, height, u1, v1, u2, v2);
}


void GFX::draw_text(const std::string& text, float x, float y, const std::string& font_name, float scale, float space_multiplier) {
    // std::cout << "Drawing text with font: " << font_name << std::endl;
    FontData font_data = get_font_data(font_name);    

    font_texture = load_texture(font_data.path);
    if (!font_texture) return;

    const float char_width = font_data.char_width;
    const float char_height = font_data.char_height;

    unsigned int font_img_width = 0, font_img_height = 0;
    std::ifstream in(font_data.path, std::ios::binary); // MUST be binary!
    
    if (in) { // Check if the file actually opened
        unsigned char buf[8];
        in.seekg(16);
        
        // Check if we actually read 8 bytes successfully
        if (in.read(reinterpret_cast<char*>(&buf), 8)) {
            font_img_width = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + (buf[3] << 0);
            font_img_height = (buf[4] << 24) + (buf[5] << 16) + (buf[6] << 8) + (buf[7] << 0);
        }
    } else {
        std::cerr << "Error: Could not open " << font_data.path << " to read dimensions." << std::endl;
    }
    
    const int chars_per_row = font_img_width / char_width;
    const int total_rows = font_img_height / char_height;

    // std::cout << "Font image dimensions: " << font_img_width << "x" << font_img_height << std::endl;


    float draw_width = char_width * scale; 
    float draw_height = char_height * scale; 

    float cursor_x = x; 

    std::string charset = font_data.charset;
    // std::cout << "Font charset: " << charset << std::endl;

    for (size_t i = 0; i < text.length(); ++i) {
        if (text[i] == ' ') {
            cursor_x += (draw_width * space_multiplier); 
            continue;
        }

        char c = text[i]; 
        size_t index;
        try{
            index = charset.find(c);
        } catch (const std::exception& e) {
            c = toupper(text[i]);
            size_t index = charset.find(c);
            if (index == std::string::npos) {
                std::cerr << "Error finding character '" << c << "' in charset: " << e.what() << std::endl;
                continue;
            }
        }
        if (index == std::string::npos) continue;

        int col = index % chars_per_row;
        int row = index / chars_per_row;

        const float tex_width = 64.0f; 
        const float tex_height = 48.0f;

        const float margin_u = (.1f / tex_width);
        const float margin_v = (.1f / tex_height);
            
        float u1 = ((float)col / chars_per_row) + margin_u;
        float v1 = ((float)row / total_rows) + margin_v;
        float u2 = ((float)(col + 1) / chars_per_row) - margin_u;
        float v2 = ((float)(row + 1) / total_rows) - margin_v;

        draw(font_texture, cursor_x, y, draw_width, draw_height, u1, v1, u2, v2);

        cursor_x += draw_width; 
    }
}