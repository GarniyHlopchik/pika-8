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
#include "globals.h"
#include "debug/debug.h"
#include "shader_utils/shader_utils.h"
#include "async_loader.h"
#include <fstream>
#include <filesystem>
#include <algorithm> 
#include <SDL3/SDL_main.h>
extern "C" int luaopen_Input(lua_State* L);
#ifdef __EMSCRIPTEN__
#include <emscripten.h>

#endif


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
    ctx->sfx->poll_loaded_sounds();
    Debug::begin_frame();
    ctx->lua->call_update(dt);
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
    auto config_future = Config::load_async();
    Config config = config_future.get();
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
    //data source setup
    #ifdef __EMSCRIPTEN__
    emscripten_async_wget("game.pika", "/game.pika", on_load_success, on_load_failure);
    return 0;
    #else
    #ifdef __ANDROID__
    FileSystem::init(EngineReadState::ZIP,"game.pika");
    #else
    if(has_embedded_zip(argv[0])){
        std::cout << "Running embedded" << std::endl;
        FileSystem::init(EngineReadState::ZIP,argv[0]);
    }else if(has_external_zip("game.pika")){
        std::cout << "Running external zip" << std::endl;
        FileSystem::init(EngineReadState::ZIP,"game.pika");
    }
    else{
        std::cout << "Running directory" << std::endl;
        FileSystem::init(EngineReadState::DIRECTORY,"");
    }
    #endif
    #endif
    //context setup------------------------
    auto config_future = Config::load_async();
    Config config = config_future.get();
    
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
    // Debug shader setup
    ctx.debugShader = make_debug_shader();
    GFX::set_debug_shader(ctx.debugShader);
    Debug::init(ctx.debugShader);
    lua.set_context(&ctx);
    
    SDL_GL_SetSwapInterval(config.get_vsync()); // Enable VSync

    bind_gfx(lua.get_state());
    bind_gfx_sprite(lua.get_state());
    bind_input(lua.get_state());
    bind_sfx(lua.get_state());
    bind_node(lua.get_state());

    /*lua_pushlightuserdata(lua.get_state(), &gfx);
    lua_setglobal(lua.get_state(), "gfx");


    luaL_requiref(lua.get_state(), "Input", luaopen_Input, 1);
    lua_pop(lua.get_state(), 1);

    std::cout << "Input module registered successfully!" << std::endl;
    */
    lua.load_script(config.get_lua_script());
    lua.call("_init");
    

    //update loop
    while(gfx.is_running()){
        main_tick(&ctx);
    }
    lua.call("_exit");

    
    return 0;
}
