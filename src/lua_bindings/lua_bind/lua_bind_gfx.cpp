#include "../lua_bindings.h"
#include "../engine_context.h"
#include "../uv_utils.h"
#include "../../gfx/gfx.h"
#include "../../gfx/text/text.h"
#include "../../gfx/sprite/sprite.h"

static int l_cls(lua_State* L){
    float r = luaL_optnumber(L,1,0.0f);
    float g = luaL_optnumber(L,2,0.0f);
    float b = luaL_optnumber(L,3,0.0f);
    glClearColor(r,g,b,1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    return 0;
}

static int l_load(lua_State* L){
    EngineContext* ctx = get_ctx(L);
    const char* path = luaL_checkstring(L, 1);

    // 1. Create a "Proxy" table: { ready = false }
    lua_newtable(L);
    lua_pushboolean(L, 0);
    lua_setfield(L, -2, "ready");
    
    // create a reference so we can find this table
    lua_pushvalue(L, -1); // duplicate the table on stack
    int registry_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    ctx->gfx->schedule_load(path, registry_ref);
    return 1;
}


static UVCoords calculate_uv(UVCoords sprite, unsigned int texture) {
    return normalize_sprite_uv(sprite, texture);
}


/*
* Lua binding for creating a sprite
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
* Returns: the sprite userdata
*/
static int l_spr(lua_State* L) {
    EngineContext* ctx = get_ctx(L);
    // 1. Get required arguments (texture, x, y)
    const unsigned int texture = luaL_checknumber(L, 1);
    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    // 2. Get optional arguments (width, height) - defaulting to 8.0f if not provided
    float width = luaL_optnumber(L, 4, 8.0f);
    float height = luaL_optnumber(L, 5, 8.0f);

    // 3. (Optional) You can also expose the UV coordinates to Lua for sprite animations
    UVCoords sprite_cut = get_sprite_cut(L, 6); // expects a table {x1, x2, y1, y2}    
    
    Color color = get_color(L, 7); // expects a table {r, g, b, a}

    // int rotation = luaL_optnumber(L, 8, 0); // TODO: if needed, input of the rotation during the rotation of the sprite can be added

    UVCoords uv = calculate_uv(sprite_cut, texture);

    // Allocate Userdata
    Sprite* s = (Sprite*)lua_newuserdata(L, sizeof(Sprite));

    // Construct the object in place
    new (s) Sprite(0,ctx->lua,ctx->scene_tree, texture, x, y, width, height, uv, color);

    // Fetch and set the pre-registered metatable
    luaL_getmetatable(L, "SpriteMeta");
    lua_setmetatable(L, -2);

    return 1;
}


static int l_getscr(lua_State* L){
    EngineContext* ctx = get_ctx(L);
    auto [width,height] = ctx->gfx->get_screen_size();
    lua_pushnumber(L,width);
    lua_pushnumber(L,height);
    return 2;
}

static void draw_text(const char* text, int x, int y, 
    std::string font_name, float scale, Color color, float space_multiplier, Text* text_obj){
    
    try {
        text_obj->draw_text(text, x, y, font_name, scale, color, space_multiplier);
    } 
    catch (const std::exception& e) {
        if (std::string(e.what()).find("Font not found") != std::string::npos) {
            font_name = "default";
            std::cerr << "Warning: " << e.what() << " Falling back to default font." << std::endl;
            try {
                text_obj->draw_text(text, x, y, font_name, scale, color, space_multiplier);
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
* 6. color (table, optional) - color of the text as a table {r=..., g=..., b=..., a=...} (default: white)
* 7. space_multiplier (number, optional) - multiplier for space character width (default: 0.4)
*/
static int l_text(lua_State* L){
    EngineContext* ctx = get_ctx(L);
    const char* text = luaL_checkstring(L,1);
    float x = luaL_checknumber(L,2);
    float y = luaL_checknumber(L,3);
    float scale = luaL_optnumber(L, 4, 1.0f);
    std::string font_name = luaL_checkstring(L, 5);
    
    Color color = get_color(L, 6);

    float space_multiplier = luaL_optnumber(L, 7, 0.4f);
    
    draw_text(text, x, y, font_name, scale, color, space_multiplier, ctx->text);
    return 0;
}

static int l_close(lua_State* L){
    EngineContext* ctx = get_ctx(L);
    ctx->gfx->close();
    return 0;
}

void bind_gfx(lua_State* L)
{
    static const luaL_Reg gfx_lib[] = {
        {"cls", l_cls},
        {"load", l_load},
        {"spr", l_spr},
        {"getscr", l_getscr},
        {"text", l_text},
        {"close", l_close},
        {NULL, NULL}
    };

    luaL_newlib(L, gfx_lib);
    lua_setglobal(L, "GFX");
}