#include "gfx.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include <algorithm>

std::vector<LoadedImages> loaded_images;
SDL_Window* GFX::window = nullptr;
SDL_GLContext GFX::gl_context = nullptr;
unsigned int GFX::shader = 0;
unsigned int GFX::debugShader = 0;
bool GFX::running = false;
void send_projection(int width, int height);
void GFX::resize(int width, int height)
{
    glViewport(0, 0, width, height);
    send_projection(width,height);
    Debug::set_projection(width, height);
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
unsigned int GFX::get_debug_shader(){
    return GFX::debugShader;
}
void GFX::set_debug_shader(unsigned int shader){
    GFX::debugShader = shader;
}
std::tuple<int,int> GFX::get_screen_size(){
    int width, height;
    SDL_GetWindowSizeInPixels(window, &width, &height);
    return std::make_tuple(width,height);
}
GFX::GFX(int w, int h, const char* title, InputState &p_state) : input_state(p_state), mobile_input_state() {
    #ifdef __linux__
    // 1. Set global hints BEFORE Init
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
    #endif
    // 2. Init
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return;
    }

    // 3. Set GL Attributes BEFORE Window Creation
    #ifdef __ANDROID__
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    #endif
    
    // Explicitly ask for a depth buffer (sometimes required for valid context)
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // 4. Create Window
    window = SDL_CreateWindow(title, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        return;
    }

    // 5. Create Context
    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        // This is where "Invalid window" was happening
        SDL_Log("Context creation failed: %s", SDL_GetError());
        return;
    }
    SDL_GL_MakeCurrent(window, gl_context);
    running = true;

    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    #ifdef __ANDROID__
    
    #else
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_Log("Failed to initialize GLAD for Desktop GL");
        return;
    }
    #endif
    SDL_GetWindowSizeInPixels(window, &w, &h);
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
    
    SDL_DestroyWindow(window);
    SDL_Quit();

}
bool GFX::is_running(){
    return GFX::running;
}
void GFX::close(){
    GFX::running = false;
}

SDL_Window* GFX::get_window() {
    return GFX::window;
}

std::vector<int> GFX::get_image_dimensions(const std::string& path) {
    int width, height, nrChannels;
    
    // 1. Get from Zip/FileSystem
    Resource res = FileSystem::get_resource(path);
    if(!res.is_valid()){
        return {0,0};
    }

    // 2. Use the "from_memory" version of STB
    int ok = stbi_info_from_memory(res.data.get(), res.size, &width, &height, &nrChannels);
    
    if(!ok){
        return {0,0};
    }
    
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


void GFX::handleMouse(const SDL_Event& event) {
    static bool mouse_left_down = false;
    static bool mouse_right_down = false;

   
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
        mouse_left_down = true;

        mobile_input::TouchPoint t;
        t.id = 999;  
        t.x = event.button.x;
        t.y = event.button.y;
        t.down = true;
        t.just_pressed = true;
        t.button = event.button.button;
        mobile_input_state.touches.push_back(t);
        mobile_input_state.num_touches++;
        mobile_input_state.is_emulating = true;

        printf("Left button DOWN at (%.0f, %.0f)\n", t.x, t.y);
    }
    else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT) {
        mouse_left_down = false;
        for (auto& t : mobile_input_state.touches) {
            if (t.id == 999) {
                t.down = false;
                t.just_released = true;
                break;
            }
        }
      
        auto it = std::remove_if(mobile_input_state.touches.begin(),
            mobile_input_state.touches.end(),
            [](const mobile_input::TouchPoint& p) { return !p.down; });
        mobile_input_state.touches.erase(it, mobile_input_state.touches.end());
        mobile_input_state.num_touches = mobile_input_state.touches.size();

        printf("Left button UP\n");
    }

  
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_RIGHT) {
        mouse_right_down = true;

        mobile_input::TouchPoint t;
        t.id = 998;  
        t.x = event.button.x;
        t.y = event.button.y;
        t.down = true;
        t.just_pressed = true;
        t.button = event.button.button;
        mobile_input_state.touches.push_back(t);
        mobile_input_state.num_touches++;
        mobile_input_state.is_emulating = true;

        printf("Right button DOWN at (%.0f, %.0f)\n", t.x, t.y);
    }
    else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_RIGHT) {
        mouse_right_down = false;
        for (auto& t : mobile_input_state.touches) {
            if (t.id == 998) {
                t.down = false;
                t.just_released = true;
                break;
            }
        }
       
        auto it = std::remove_if(mobile_input_state.touches.begin(),
            mobile_input_state.touches.end(),
            [](const mobile_input::TouchPoint& p) { return !p.down; });
        mobile_input_state.touches.erase(it, mobile_input_state.touches.end());
        mobile_input_state.num_touches = mobile_input_state.touches.size();

        printf("Right button UP\n");
    }

  
    else if (event.type == SDL_EVENT_MOUSE_MOTION) {
        if (mouse_left_down) {
            for (auto& t : mobile_input_state.touches) {
                if (t.id == 999) {
                    t.dx = event.motion.x - t.x;
                    t.dy = event.motion.y - t.y;
                    t.x = event.motion.x;
                    t.y = event.motion.y;
                    break;
                }
            }
        }
        if (mouse_right_down) {
            for (auto& t : mobile_input_state.touches) {
                if (t.id == 998) {
                    t.dx = event.motion.x - t.x;
                    t.dy = event.motion.y - t.y;
                    t.x = event.motion.x;
                    t.y = event.motion.y;
                    break;
                }
            }
        }
    }
}
// void GFX::handleTouch(const SDL_Event& event) {
//     const auto& tf = event.tfinger;
//     int w, h;
//     SDL_GetWindowSizeInPixels(window, &w, &h);

<<<<<<< HEAD
    if (event.type == SDL_EVENT_FINGER_DOWN) {
        mobile_input::TouchPoint t;
        t.id = tf.fingerID;           
        t.x = tf.x * w;                
        t.y = tf.y * h;
        t.down = true;
        t.just_pressed = true;
        t.button = 0;
        mobile_input_state.touches.push_back(t);
        mobile_input_state.num_touches++;
        printf("Finger DOWN: id=%lld at (%.0f, %.0f)\n", (long long)t.id, t.x, t.y);
    }
    else if (event.type == SDL_EVENT_FINGER_UP) {
        for (auto& t : mobile_input_state.touches) {
            if (t.id == tf.fingerID) {
                t.down = false;
                t.just_released = true;
                break;
            }
        }
       
        auto it = std::remove_if(mobile_input_state.touches.begin(),
            mobile_input_state.touches.end(),
            [](const mobile_input::TouchPoint& p) { return !p.down; });
        mobile_input_state.touches.erase(it, mobile_input_state.touches.end());
        mobile_input_state.num_touches = mobile_input_state.touches.size();
        printf("Finger UP: id=%lld\n", (long long)tf.fingerID);
    }
    else if (event.type == SDL_EVENT_FINGER_MOTION) {
        for (auto& t : mobile_input_state.touches) {
            if (t.id == tf.fingerID) {
                float nx = tf.x * w;
                float ny = tf.y * h;
                t.dx = nx - t.x;
                t.dy = ny - t.y;
                t.x = nx;
                t.y = ny;
                break;
            }
        }
    }
}
=======
//     if (event.type == SDL_EVENT_FINGER_DOWN) {
//         mobile_input::TouchPoint t;
//         t.id = tf.fingerID;           
//         t.x = tf.x * w;                
//         t.y = tf.y * h;
//         t.down = true;
//         t.just_pressed = true;
//         t.button = 0;
//         mobile_input_state.touches.push_back(t);
//         mobile_input_state.num_touches++;
//         printf("Finger DOWN: id=%d at (%.0f, %.0f)\n", t.id, t.x, t.y);
//     }
//     else if (event.type == SDL_EVENT_FINGER_UP) {
//         for (auto& t : mobile_input_state.touches) {
//             if (t.id == tf.fingerID) {
//                 t.down = false;
//                 t.just_released = true;
//                 break;
//             }
//         }
       
//         auto it = std::remove_if(mobile_input_state.touches.begin(),
//             mobile_input_state.touches.end(),
//             [](const mobile_input::TouchPoint& p) { return !p.down; });
//         mobile_input_state.touches.erase(it, mobile_input_state.touches.end());
//         mobile_input_state.num_touches = mobile_input_state.touches.size();
//         printf("Finger UP: id=%d\n", tf.fingerID);
//     }
//     else if (event.type == SDL_EVENT_FINGER_MOTION) {
//         for (auto& t : mobile_input_state.touches) {
//             if (t.id == tf.fingerID) {
//                 float nx = tf.x * w;
//                 float ny = tf.y * h;
//                 t.dx = nx - t.x;
//                 t.dy = ny - t.y;
//                 t.x = nx;
//                 t.y = ny;
//                 break;
//             }
//         }
//     }
// }
>>>>>>> dede4b9f8408e860a190933d8f1b3fa019aa2ebd
void GFX::update(){

    mobile_input_state.resetFrame();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                GFX::running = false;
                break;

            case SDL_EVENT_KEY_DOWN:
                input_state.keys[event.key.scancode] = true;
                break;

            case SDL_EVENT_KEY_UP:
                input_state.keys[event.key.scancode] = false;
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                handleMouse(event);
                break;

            case SDL_EVENT_MOUSE_MOTION:
                input_state.mouseX = event.motion.x;
                input_state.mouseY = event.motion.y;
                handleMouse(event);
                break;

            case SDL_EVENT_FINGER_DOWN:
            case SDL_EVENT_FINGER_UP:
            case SDL_EVENT_FINGER_MOTION:
                handleTouch(event);
                break;

            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                resize(event.window.data1, event.window.data2);
                break;
        }
    }

    // Flush debug after scene update but before main render
    Debug::flush(GFX::debugShader);

    spritemesh.flush(shader);
    SDL_GL_SwapWindow(window);
    //set previous input to this for next frame
    std::copy(std::begin(input_state.keys), std::end(input_state.keys), std::begin(input_state.previous_keys));
}



unsigned int GFX::load_texture(const std::string& path){
    
    int width, height, nrChannels;

    Resource res = FileSystem::get_resource(path);
    if(!res.is_valid()){
        return 0;
    }

    unsigned char *texture_data = stbi_load_from_memory(res.data.get(),res.size, &width, &height, &nrChannels, 0);
    if(!texture_data){
        return 0;
    }
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texture_data);
    // set nearest-neighbor filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // optionally, set wrap mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(texture_data);

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

void default_color_helper(Color* color){
    Color white = {1.0f, 1.0f, 1.0f, 1.0f};
    Color black = {0.0f, 0.0f, 0.0f, 1.0f};

    if (*color > white) *color = white;
    if (*color < black) *color = black;
}

void GFX::draw(const Sprite& sprite) {
    spritemesh.draw(shader, sprite);
}
