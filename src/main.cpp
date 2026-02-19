#include "config.h"

#include "lua_system/lua_system.h"
#include "gfx/gfx.h"
#include "user_input/user_input.h"
#include "json_reader/get_config.h"
Config config;

GFX gfx(config.get_window_width(),config.get_window_height(), config.get_window_title().c_str());

int l_btn(lua_State* L){
    int key = luaL_checknumber(L,1);
    bool result = IsKeyPressed(key);
    lua_pushboolean(L,result);
    return 1;
}
int l_btnp(lua_State* L){
    int key = luaL_checknumber(L,1);
    bool result = IsKeyDown(key);
    lua_pushboolean(L,result);
    return 1;
}
int l_mpos(lua_State* L){
    double* position = getRelativeCursorPosition();
    lua_pushnumber(L,position[0]);
    lua_pushnumber(L,position[1]);
    delete[] position;
    return 2;
}
int l_load(lua_State* L){
    const char* path = luaL_checkstring(L, 1);
    const unsigned int id = gfx.load_texture(path);
    lua_pushnumber(L,id);
    return 1;
}
int l_spr(lua_State* L){
    const unsigned int texture = luaL_checknumber(L,1);
    float x = luaL_checknumber(L,2);
    float y = luaL_checknumber(L,3);
    gfx.draw(texture,x,y);
    return 0;
}
int l_cls(lua_State* L){
    glClear(GL_COLOR_BUFFER_BIT);
    return 0;
}
int main(){

    //opengl setup------------------------
    
    //lua setup-----------------------------
    LuaSystem lua;

    static const luaL_Reg gfx_lib[] = {
    {"cls", l_cls},
    {"load", l_load},
    {"spr", l_spr},
    {NULL, NULL}
    };
    lua.bind_lib(gfx_lib,"GFX");

    static const luaL_Reg input_lib[] = {
    {"btn", l_btn},
    {"btnp", l_btnp},
    {"mpos", l_mpos},
    {NULL, NULL}
    };
    lua.bind_lib(input_lib,"Input");

    lua.load_script(config.get_lua_script());
    lua.call("_init");
    //----------------------

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
    return 0;
}
