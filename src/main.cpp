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
#include <fstream>
#include <filesystem>
#include <algorithm> 

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

int main(int argc, char** argv){
    //data source setup
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
    //context setup------------------------
    Config config;
    InputState input_state;
    GFX gfx(config.get_window_width(),config.get_window_height(), config.get_window_title().c_str(),input_state);
    Text text(gfx);
    SFX sfx;
    LuaSystem lua;
    SceneTree scene_tree(&lua);
    EngineContext ctx = {
        &lua,
        &scene_tree,
        &gfx,
        &sfx,
        &text,
        &config,
        &input_state
    };
    lua.set_context(&ctx);
    

    bind_gfx(lua.get_state());
    bind_input(lua.get_state());
    bind_sfx(lua.get_state());
    bind_node(lua.get_state());

    lua.load_script(config.get_lua_script());
    lua.call("_init");

    
    //update loop
    auto last_time = std::chrono::high_resolution_clock::now();
    while(gfx.is_running()){
        //count delta
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> delta = now - last_time;
        last_time = now;
        float dt = delta.count();
        lua.call_update(dt);
        gfx.update();
    }
    lua.call("_exit");
    return 0;
}
