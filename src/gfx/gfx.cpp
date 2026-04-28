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
    send_projection(width, height);
    Debug::set_projection(width, height);
}
void send_projection(int width, int height) {
    unsigned int shader = GFX::get_shader();
    glUseProgram(shader);
    unsigned int location = glGetUniformLocation(shader, "uProjection");
    float proj[16] = {
    2.0f / width,     0,        0, -1,
    0,        -2.0f / height,  0,  1,
    0,             0,        -1,  0,
    0,             0,         0,  1
    };

    glUniformMatrix4fv(location, 1, GL_TRUE, proj);

}
unsigned int GFX::get_shader() {
    return GFX::shader;
}
unsigned int GFX::get_debug_shader() {
    return GFX::debugShader;
}
void GFX::set_debug_shader(unsigned int shader) {
    GFX::debugShader = shader;
}
std::tuple<int, int> GFX::get_screen_size() {
    int width, height;
    SDL_GetWindowSizeInPixels(window, &width, &height);
    return std::make_tuple(width, height);
}
GFX::GFX(int w, int h, const char* title, InputState& p_state) : input_state(p_state), lua(nullptr), worker_should_run(false) {
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
    glViewport(0, 0, w, h);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glClearColor(0.11f,0.79f,0.88f,1.0f);
    shader = make_shader();
    send_projection(w, h);

    spritemesh.init();


}

GFX::~GFX() {
    worker_should_run = false;
    glDeleteProgram(shader);

    SDL_DestroyWindow(window);
    SDL_Quit();

}
bool GFX::is_running() {
    return GFX::running;
}
void GFX::close() {
    GFX::running = false;
}

SDL_Window* GFX::get_window() {
    return GFX::window;
}

std::vector<int> GFX::get_image_dimensions(const std::string& path) {
    int width, height, nrChannels;

    // 1. Get from Zip/FileSystem
    Resource res = FileSystem::get_resource(path);
    if (!res.is_valid()) {
        return { 0,0 };
    }

    // 2. Use the "from_memory" version of STB
    int ok = stbi_info_from_memory(res.data.get(), res.size, &width, &height, &nrChannels);

    if (!ok) {
        return { 0,0 };
    }

    return { width, height };
}
std::vector<int> GFX::get_image_dimensions(const unsigned int texture) {
    return get_image_dimensions(get_texture_path(texture));
}

static unsigned int upload_pixels_to_gpu(const unsigned char* pixels, int width, int height, int channels) {
    if (!pixels) return 0;

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return texture;
}

std::string GFX::get_texture_path(const unsigned int id) {
    for (const LoadedImages& img : loaded_images) {
        if (img.id == id) {
            return img.path;
        }
    }
    throw std::runtime_error("Image not found with id: " + id);
}

void GFX::add_new_image(const LoadedImages img) {
    loaded_images.push_back(img);
}



void GFX::update() {

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


// Sync version of texture loading
// Why? beacause i dont want to bother and rewrite text display
unsigned int GFX::load_texture(const std::string& path) {
    int width, height, nrChannels;
    Resource res = FileSystem::get_resource(path);
    if (!res.is_valid()) return 0;

    unsigned char* texture_data = stbi_load_from_memory(res.data.get(), res.size, &width, &height, &nrChannels, 0);
    if (!texture_data) return 0;

    unsigned int texture = upload_pixels_to_gpu(texture_data, width, height, nrChannels);
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

void default_color_helper(Color* color) {
    Color white = { 1.0f, 1.0f, 1.0f, 1.0f };
    Color black = { 0.0f, 0.0f, 0.0f, 1.0f };

    if (*color > white) *color = white;
    if (*color < black) *color = black;
}


// async texture loading
void GFX::set_lua_system(LuaSystem* l_lua) {
    lua = l_lua;
    worker_should_run = true;
    worker_thread = std::jthread(&GFX::worker_loop, this);
}

unsigned int GFX::get_texture_id(){
    static unsigned int id = 0;
    return id++;
}

static ImageData load_image_data(const std::string& path) {
    ImageData result = {nullptr, 0, 0, 0};
    
    Resource res = FileSystem::get_resource(path);
    if (!res.is_valid()) {
        return result;
    }

    int width, height, nrChannels;
    unsigned char* texture_data = stbi_load_from_memory(res.data.get(), res.size, &width, &height, &nrChannels, 0);
    if (!texture_data) {
        return result;
    }

    result.pixel_data = texture_data;
    result.width = width;
    result.height = height;
    result.channels = nrChannels;
    
    return result;
}

static unsigned int create_texture_from_image_data(const ImageData& image) {
    return upload_pixels_to_gpu(image.pixel_data, image.width, image.height, image.channels);
}

void GFX::schedule_load(const std::string& path, int registry_ref){
    std::lock_guard<std::mutex> lock(to_worker_mutex);
    TextureLoadRequest request = {registry_ref, path};
    to_worker_queue.enqueue(request, 1);
    cv.notify_one();
}

void GFX::poll_loaded_textures(){
    std::lock_guard<std::mutex> lock(to_main_mutex);
    while(!to_main_queue.empty()){
        TextureLoadResult finished_task = to_main_queue.dequeue(QueueProtocol::OLDEST).data;
        
        // create texture on main thread
        unsigned int texture_id = create_texture_from_image_data(finished_task.image);
        
        // add to loaded images tracking
        LoadedImages img;
        img.id = texture_id;
        img.path = finished_task.path;
        GFX::add_new_image(img);
        
        // Free the pixel data
        if (finished_task.image.pixel_data) {
            stbi_image_free(finished_task.image.pixel_data);
        }
        
        lua->resolve_promise(texture_id, finished_task.registry_ref);
    }
}

void GFX::worker_loop(){
    while(worker_should_run){
        TextureLoadRequest task;
        
        {
            std::unique_lock<std::mutex> lock(to_worker_mutex);
            
            // wait until there is a task or the engine is shutting down
            cv.wait(lock, [this]{ 
                return to_worker_queue.peek(QueueProtocol::OLDEST).priority || !worker_should_run; 
            });

            // if we woke up because we're sinking, break the loop
            if (!worker_should_run && !to_worker_queue.peek(QueueProtocol::OLDEST).priority) break;

            task = to_worker_queue.dequeue(QueueProtocol::OLDEST).data;
        }

        // work on this if it is. is it working because it is or does it is to work?
        if(!task.path.empty()) 
        {
            ImageData image = load_image_data(task.path);
            
            // enqueue to main with lock
            {
                std::lock_guard<std::mutex> lock(to_main_mutex);
                TextureLoadResult data = {task.registry_ref, 0, image, task.path};
                to_main_queue.enqueue(data, 1);
            }
        }
        
    }
}

void GFX::draw(const Sprite& sprite) {
    spritemesh.draw(shader, sprite);
}