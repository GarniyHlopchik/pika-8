#include "config.h"
#include "gfx/gfx.h"
#include "sfx/sfx.h"
#include "json_reader/get_config.h"
#include "scene_tree/scene_tree.h"
#include "lua_bindings/lua_bindings.h"
#include "lua_bindings/engine_context.h"




int main(){

    //context setup------------------------
    Config config;
    GFX gfx(config.get_window_width(),config.get_window_height(), config.get_window_title().c_str());
    SFX sfx;
    LuaSystem lua;
    SceneTree scene_tree(&lua);
    EngineContext ctx = {
        &lua,
        &scene_tree,
        &gfx,
        &sfx,
        &config
    };
    lua.set_context(&ctx);
    //lua setup-----------------------------

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
