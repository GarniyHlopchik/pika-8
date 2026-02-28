#include "config.h"


#include "gfx/gfx.h"
#include "user_input/user_input.h"
#include "json_reader/get_config.h"
#include "sfx/sfx.h"
#include "scene_tree/scene_tree.h"
Config config;

GFX gfx(config.get_window_width(),config.get_window_height(), config.get_window_title().c_str());
SFX sfx;
LuaSystem lua;
SceneTree scene_tree(&lua);

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
    
    LoadedImages img;
    img.id = id;
    img.path = path;
    GFX::add_new_image(img);

    std::cout << "Loaded id " << id << std::endl;
    lua_pushnumber(L,id);
    return 1;
}

struct SpriteCut{
    float x1, x2, y1, y2;
};

UVCoords calculate_uv(SpriteCut sprite, unsigned int texture) {
    UVCoords uv;
    if (sprite.x2 > sprite.x1 && sprite.y2 > sprite.y1) {
        std::vector<int> dims = GFX::get_image_dimensions(texture);
        float tex_w = (float)dims[0];
        float tex_h = (float)dims[1];

        // Protect against division by zero
        if (tex_w > 0.0f && tex_h > 0.0f) {
            // UVs are normalized (0.0 to 1.0). Divide the pixel coordinate by the total dimension.
            uv.u1 = sprite.x1 / tex_w;
            uv.v1 = sprite.y1 / tex_h;
            uv.u2 = sprite.x2 / tex_w;
            uv.v2 = sprite.y2 / tex_h;
        } else {
            // Fallback if the texture fails to load properly
            uv.u1 = 0.0f; uv.v1 = 0.0f; uv.u2 = 1.0f; uv.v2 = 1.0f;
        }
    } else {
        // Default UV coordinates (draws the entire texture)
        uv.u1 = 0.0f; uv.v1 = 0.0f; 
        uv.u2 = 1.0f; uv.v2 = 1.0f;
    }
    return uv;
}

/*
* Lua binding for drawing a sprite
* Arguments:
* 1. texture (number) - the texture ID to draw
* 2. x (number) - destination x position on screen
* 3. y (number) - destination y position on screen
* 4. width (number, optional) - destination width on screen (default: 8)
* 5. height (number, optional) - destination height on screen (default: 8)
* 6. sprite_x1 (number, optional) - starting X pixel on the sprite sheet (left)
* 7. sprite_x2 (number, optional) - ending X pixel on the sprite sheet (right)
* 8. sprite_y1 (number, optional) - starting Y pixel on the sprite sheet (top)
* 9. sprite_y2 (number, optional) - ending Y pixel on the sprite sheet (bottom)
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
    SpriteCut sprite;
    sprite.x1 = luaL_optnumber(L, 6, 0.0f); // Left
    sprite.x2 = luaL_optnumber(L, 7, 0.0f); // Right
    sprite.y1 = luaL_optnumber(L, 8, 0.0f); // Top
    sprite.y2 = luaL_optnumber(L, 9, 0.0f); // Bottom 
    
    UVCoords uv = calculate_uv(sprite, texture);

    // std::cout << u1 << v1 << u2 << v2 << std::endl;

    // Pass everything to your updated C++ draw function
    gfx.draw(texture, x, y, width, height, uv);
    
    return 0;
}
int l_cls(lua_State* L){
    float r = luaL_optnumber(L,1,0.0f);
    float g = luaL_optnumber(L,2,0.0f);
    float b = luaL_optnumber(L,3,0.0f);
    glClearColor(r,g,b,1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    return 0;
}
int l_getscr(lua_State* L){
    auto [width,height] = gfx.get_screen_size();
    lua_pushnumber(L,width);
    lua_pushnumber(L,height);
    return 2;
}

void draw_text(const char* text, int x, int y, std::string font_name, float scale, float space_multiplier){
    try {
        gfx.draw_text(text, x, y, font_name, scale, space_multiplier);
    } 
    catch (const std::exception& e) {
        if (std::string(e.what()).find("Font not found") != std::string::npos) {
            font_name = "default";
            std::cerr << "Warning: " << e.what() << " Falling back to default font." << std::endl;
            try {
                gfx.draw_text(text, x, y, font_name, scale, space_multiplier);
            } catch (const std::exception& e) {
                std::cerr << "Error: Default font also not found. Cannot draw text." << std::endl;
            }
        } else {
            std::cerr << "Error in l_text: " << e.what() << std::endl;
        }
    }
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
    float scale = luaL_optnumber(L, 4, 1.0f);
    std::string font_name = luaL_checkstring(L, 5);
    float space_multiplier = luaL_optnumber(L, 6, 0.4f);

    
    draw_text(text, x, y, font_name, scale, space_multiplier);
    return 0;
}

//// SFX Lua bindings
int l_sfx_load(lua_State* L){
    const char* path = luaL_checkstring(L,1);
    unsigned int id = sfx.load(path);
    lua_pushnumber(L,id);
    return 1;
}
/*
* Lua binding for playing a sound effect
* Arguments:
* 1. sound_id (number) - the ID of the sound to play
* 2. volume (number, optional) - volume of the sound (default: 100.0)
* 3. pitch (number, optional) - pitch of the sound (default: 1.0)
* 4. loop (boolean, optional) - whether to loop the sound (default: false)
* 7. pan (number, optional) - stereo pan of the sound (-1.0 for left, 1.0 for right, default: 0.0)
*/
int l_sfx_play(lua_State* L){
    unsigned int id = luaL_checknumber(L,1);
    float volume = luaL_optnumber(L, 2, 100.0f);
    float pitch = luaL_optnumber(L, 3, 1.0f);
    bool loop = lua_toboolean(L, 4);    
    float pan = luaL_optnumber(L, 7, 0.0f);
    sfx.play(id, volume, pitch, loop, pan);
    return 0;
}

int l_sfx_stop(lua_State* L){
    unsigned int id = luaL_checknumber(L,1);
    sfx.stop(id);
    return 0;
}

int main(){

    //opengl setup------------------------
    
    //lua setup-----------------------------
    

    static const luaL_Reg gfx_lib[] = {
    {"cls", l_cls},
    {"load", l_load},
    {"spr", l_spr},
    {"text", l_text},
    {"getscr",l_getscr},
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

    static const luaL_Reg sfx_lib[] = {
        {"load",l_sfx_load},
        {"play",l_sfx_play},
        {"stop",l_sfx_stop},
        {NULL,NULL}
    };
    lua.bind_lib(sfx_lib,"SFX");


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
