#include "config.h"

#include "lua_system/lua_system.h"
#include "gfx/gfx.h"

#include "json_reader/get_config.h"
Config config;

GFX gfx(config.get_window_width(),config.get_window_height());

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
    std::cout << "Screen cleared" << std::endl;
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

    lua.load_script(config.get_lua_script());
    lua.call("_init");
    //----------------------

    //draw sprite obj
    //SpriteMesh* sprite = new SpriteMesh();


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
