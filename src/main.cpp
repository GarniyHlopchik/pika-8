#include "config.h"
#include "gfx/gfx.h"
#include "sfx/sfx.h"
#include "gfx/text/text.h"
#include "json_reader/get_config.h"
#include "scene_tree/scene_tree.h"
#include "lua_bindings/lua_bindings.h"
#include "lua_bindings/engine_context.h"
#include "file_resolve/file_system.h"
#include "globals.h"



int main(int argc, char** argv){
    //data source setup
    if(std::string(argv[argc-1]) == "ZIP"){
        FileSystem::init(EngineReadState::ZIP,"game.pika");
    }
    else{
        FileSystem::init(EngineReadState::DIRECTORY,"");
    }
    //context setup------------------------
    Config config;
    GFX gfx(config.get_window_width(),config.get_window_height(), config.get_window_title().c_str());
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
        &config
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
    while(!gfx.window_should_close()){
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
