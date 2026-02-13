#include "config.h"
#include "sprite_mesh/mesh.h"
#include "lua_system/lua_system.h"
#include "gfx/gfx.h"

int l_cls(lua_State* L){
    glClear(GL_COLOR_BUFFER_BIT);
    std::cout << "Screen cleared" << std::endl;
    return 0;
}
int main(){
    //opengl setup------------------------
    GFX gfx(512,512);
    //lua setup-----------------------------
    LuaSystem lua;

    static const luaL_Reg gfx_lib[] = {
    {"cls", l_cls},
    {NULL, NULL}
    };
    lua.bind_lib(gfx_lib,"GFX");

    lua.load_script("main.lua");
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
