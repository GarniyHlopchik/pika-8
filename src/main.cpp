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
    std::cout << "Loading " << path << std::endl;
    const unsigned int id = gfx.load_texture(path);
    std::cout << "Loaded id " << id << std::endl;
    lua_pushnumber(L,id);
    return 1;
}
/*
* Lua binding for drawing a sprite
* Arguments:
* 1. texture (number) - the texture ID to draw
* 2. x (number) - x position
* 3. y (number) - y position
* 4. width (number, optional) - width of the sprite (default: 8)
* 5. height (number, optional) - height of the sprite (default: 8)
* 6. u1 (number, optional) - left UV coordinate (default: 0)
* 7. v1 (number, optional) - top UV coordinate (default: 0)
* 8. u2 (number, optional) - right UV coordinate (default: 1)
* 9. v2 (number, optional) - bottom UV coordinate (default: 1)  
*/
int l_spr(lua_State* L) {
    // 1. Get required arguments (texture, x, y)
    const unsigned int texture = luaL_checknumber(L, 1);
    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    // 2. Get optional arguments (width, height) - defaulting to 8.0f if not provided
    float width = luaL_optnumber(L, 4, 8.0f);
    float height = luaL_optnumber(L, 5, 8.0f);

    // 3. (Optional) You can also expose the UV coordinates to Lua for sprite animations
    float u1 = luaL_optnumber(L, 6, 0.0f);
    float v1 = luaL_optnumber(L, 7, 0.0f);
    float u2 = luaL_optnumber(L, 8, 1.0f);
    float v2 = luaL_optnumber(L, 9, 1.0f);

    // Pass everything to your updated C++ draw function
    gfx.draw(texture, x, y, width, height, u1, v1, u2, v2);
    
    return 0;
}
int l_cls(lua_State* L){
    glClear(GL_COLOR_BUFFER_BIT);
    return 0;
}

/*
* Lua binding for drawing text
* Arguments:
* 1. text (string) - the text to draw
* 2. x (number) - x position
* 3. y (number) - y position
* 4. font_name (string) - name of the font to use
* 5. scale (number, optional) - scale of the text (default: 1.0)
* 6. space_multiplier (number, optional) - multiplier for space character width (default: 0.4)
*/
int l_text(lua_State* L){
    const char* text = luaL_checkstring(L,1);
    float x = luaL_checknumber(L,2);
    float y = luaL_checknumber(L,3);
    std::string font_name = luaL_checkstring(L, 4);
    float scale = luaL_optnumber(L, 5, 1.0f);
    float space_multiplier = luaL_optnumber(L, 6, 0.4f);
    gfx.draw_text(text,x, y, font_name, scale, space_multiplier);
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
    {"text", l_text},
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
