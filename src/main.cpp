#include "config.h"
#include "gfx/gfx.h"
#include "sfx/sfx.h"
#include "gfx/text/text.h"
#include "json_reader/get_config.h"
#include "scene_tree/scene_tree.h"
#include "lua_bindings/lua_bindings.h"
#include "lua_bindings/engine_context.h"
#include "file_resolve/file_system.h"
#include "user_input/user_input.h"
#include "user_input/button/button.h"
#include "globals.h"
#include "debug/debug.h"
#include "shader_utils/shader_utils.h"
#include "async_loader.h"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <queue>
#include <SDL3/SDL_main.h>
#include "logger/proxy.h"
#include "debug/framecounter.h"
extern "C" int luaopen_Input(lua_State* L);
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef _WIN32
    #include <windows.h>
    #include <iostream>
#endif

void enableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

bool has_embedded_zip(const std::string& exe_path)
{
    std::ifstream file(exe_path, std::ios::binary | std::ios::ate);
    if (!file) return false;

    auto size = file.tellg();
    size_t read_size = std::min<size_t>(static_cast<size_t>(size), 65536);
    
    // Cast read_size to streamoff to resolve the ambiguity
    file.seekg(size - static_cast<std::streamoff>(read_size));

    std::vector<char> buf(read_size);
    file.read(buf.data(), read_size);

    for (size_t i = 0; i < read_size - 4; i++) {
        if ((unsigned char)buf[i] == 0x50 &&
            (unsigned char)buf[i+1] == 0x4B &&
            (unsigned char)buf[i+2] == 0x05 &&
            (unsigned char)buf[i+3] == 0x06) {
            return true;
        }
    }

    return false;
}
bool has_external_zip(const std::string& path)
{
    return std::filesystem::exists(path);
}

// Early startup logging queue - used before ProxyLogger is initialized
struct EarlyLog {
    LogLevel level;
    std::string message;
};
std::queue<EarlyLog> early_log_queue;

void early_log(LogLevel level, const std::string& message) {
    early_log_queue.push({level, message});
}

void flush_early_logs() {
    while (!early_log_queue.empty()) {
        const auto& log_entry = early_log_queue.front();
        ProxyLogger::getInstance().log(log_entry.level, log_entry.message);
        early_log_queue.pop();
    }
}

auto last_time = std::chrono::high_resolution_clock::now();
EngineContext ctx;
void main_tick(void* arg) {
    // Cast the generic pointer back to your struct
    EngineContext* ctx = static_cast<EngineContext*>(arg);

    // 1. Delta time logic
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> delta = now - last_time;
    last_time = now;
    float dt = delta.count();

    // 2. Run the frame

    // Poll input and update input state for async 
    ctx->sfx->poll_loaded_sounds();
    ctx->gfx->poll_loaded_textures();

    Debug::begin_frame();
    
    // Update button states and invoke callbacks
    button::button_update_all(*(ctx->input_state));
    
    ctx->lua->call_update(dt);
    
    // Draw all buttons
    button::button_draw_all(ctx->gfx);
    
    ctx->gfx->update();
    AsyncLoader::process_main_thread_tasks();
    
    // 3. Handle quitting
    #ifdef __EMSCRIPTEN__
    if (!ctx->gfx->is_running()) {
        ctx->lua->call("_exit");
        emscripten_cancel_main_loop();
    }
    #endif
}
#ifdef __EMSCRIPTEN__
void on_load_success(const char* file){
    FileSystem::init(EngineReadState::ZIP,"game.pika");
     //context setup------------------------
    Config config;
    // auto config_future = Config::load_async();
    // Config config = config_future.get();
    InputState input_state;
    GFX gfx(config.get_window_width(),config.get_window_height(), config.get_window_title().c_str(),input_state);
    Text text(gfx);
    LuaSystem lua;
    SFX sfx(&lua);
    
    SceneTree scene_tree(&lua);
    ctx = {
        &lua,
        &scene_tree,
        &gfx,
        &sfx,
        &text,
        &config,
        0,
        &input_state
    };
    ctx.debugShader = make_debug_shader();
    GFX::set_debug_shader(ctx.debugShader);
    Debug::init(ctx.debugShader);
    lua.set_context(&ctx);
    gfx.set_lua_system(&lua);
    
    initTouchInput();
    auto [w, h] = gfx.get_screen_size();
    setTouchWindowSize(w, h);

    bind_gfx(lua.get_state());
    bind_input(lua.get_state());
    bind_sfx(lua.get_state());
    bind_node(lua.get_state());

   /* lua_pushlightuserdata(lua.get_state(), &gfx);
    lua_setglobal(lua.get_state(), "gfx");
    luaL_requiref(lua.get_state(), "Input", luaopen_Input, 1);
    lua_pop(lua.get_state(), 1);*/


    lua.load_script(config.get_lua_script());
    lua.call("_init");

    emscripten_set_main_loop_arg(main_tick, &ctx, 0, true);
}
void on_load_failure(const char* file){

}
#endif
int main(int argc, char** argv){
	enableANSI();
    //data source setup - MUST be before Config object creation
    early_log(LogLevel::INFO, "=== PIKA-8 Engine Starting ===");
    early_log(LogLevel::DEBUG, "Main entry point reached with " + std::to_string(argc) + " arguments");
    #ifdef __EMSCRIPTEN__
    early_log(LogLevel::DEBUG, "Running on Emscripten platform");
    emscripten_async_wget("game.pika", "/game.pika", on_load_success, on_load_failure);
    return 0;
    #else
    #ifdef __ANDROID__
    early_log(LogLevel::DEBUG, "Running on Android platform");
    FileSystem::init(EngineReadState::ZIP,"game.pika");
    #else
    early_log(LogLevel::DEBUG, "Detecting game data source...");
    if(has_embedded_zip(argv[0])){
		early_log(LogLevel::DEBUG, "Running embedded");
        FileSystem::init(EngineReadState::ZIP,argv[0]);
    }else if(has_external_zip("game.pika")){
        early_log(LogLevel::DEBUG, "Running external zip");
        FileSystem::init(EngineReadState::ZIP,"game.pika");
    }
    else{
        early_log(LogLevel::DEBUG, "Running directory");
        FileSystem::init(EngineReadState::DIRECTORY,"");
    }
    #endif
    #endif

	early_log(LogLevel::DEBUG, "Loading configuration...");
	Config config;
	ProxyLogger::getInstance().init(config.get_logger_data());
	flush_early_logs();
	LOG(LogLevel::WARNING, "Realtime logging initialized. Previous early log messages flushed to real logger");

    LOG(LogLevel::DEBUG, "Initializing input system...");
    InputState input_state;
    LOG(LogLevel::DEBUG, "Creating graphics context...");
    GFX gfx(config.get_window_width(),config.get_window_height(), config.get_window_title().c_str(),input_state);
    LOG(LogLevel::DEBUG, "Window size set to ", config.get_window_width(), "x", config.get_window_height());

	LOG(LogLevel::DEBUG, "Initializing text rendering...");
	Text text(gfx);
    LOG(LogLevel::DEBUG, "Initializing Lua system...");
    LuaSystem lua;
    LOG(LogLevel::DEBUG, "Initializing audio system...");
    SFX sfx(&lua);
    



    SceneTree scene_tree(&lua);
    ctx = {
        &lua,
        &scene_tree,
        &gfx,
        &sfx,
        &text,
        &config,
        0,
        &input_state
    };
    // Debug shader setup
    LOG(LogLevel::DEBUG, "Creating debug shader...");
    ctx.debugShader = make_debug_shader();
    GFX::set_debug_shader(ctx.debugShader);
    Debug::init(ctx.debugShader);
    lua.set_context(&ctx);
    gfx.set_lua_system(&lua);
    LOG(LogLevel::DEBUG, "Setting VSync: ", config.get_vsync() ? "enabled" : "disabled");
    SDL_GL_SetSwapInterval(config.get_vsync()); // Enable VSync

    initTouchInput();
    auto [w, h] = gfx.get_screen_size();
    setTouchWindowSize(w, h);

    LOG(LogLevel::DEBUG, "Registering Lua bindings...");
    bind_gfx(lua.get_state());
    bind_gfx_sprite(lua.get_state());
    bind_input(lua.get_state());
    bind_sfx(lua.get_state());
    bind_node(lua.get_state());
    bind_debug(lua.get_state());

    /*lua_pushlightuserdata(lua.get_state(), &gfx);
    lua_setglobal(lua.get_state(), "gfx");


    luaL_requiref(lua.get_state(), "Input", luaopen_Input, 1);
    lua_pop(lua.get_state(), 1);

    std::cout << "Input module registered successfully!" << std::endl;
    */
    lua.load_script(config.get_lua_script());
    lua.call("_init");
    
    #ifdef _WIN32
    if (config.get_show_console()) {
        AllocConsole();
		enableANSI();
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
		LOG(LogLevel::DEBUG, "Console enabled");
    } else {
        FreeConsole();
    }
    #endif

	LOG(LogLevel::INFO, "=== Engine initialization complete ===");
	LOG(LogLevel::INFO, "Starting main loop...");
    //update loop
	FrameCounter frame;
	frame.init();
    while(gfx.is_running()){
        frame.add_frame();
        resetTouchFrame();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                gfx.close();
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                handleMouseEvent(event);
                break;

            case SDL_EVENT_MOUSE_MOTION:
                input_state.mouseX = event.motion.x;
                input_state.mouseY = event.motion.y;
                handleMouseEvent(event);
                break;

            case SDL_EVENT_FINGER_DOWN:
            case SDL_EVENT_FINGER_UP:
            case SDL_EVENT_FINGER_MOTION:
                handleTouchEvent(event);
                break;

            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                setTouchWindowSize(event.window.data1, event.window.data2);
                gfx.resize(event.window.data1, event.window.data2);
                break;

            default:
                break;
            }


        }
        main_tick(&ctx);
    }

    LOG(LogLevel::INFO, "Main loop finished after ", frame.get_frame_count(), " frames");
    LOG(LogLevel::INFO, "Calling Lua _exit function...");
    AllocConsole();
    enableANSI();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    lua.call("_exit");
    
    LOG(LogLevel::INFO, "=== PIKA-8 Engine Shutting Down ===");
    exit(0);
    return 0;
}
