#include "../lua_bindings.h"
#include "../../gfx/sprite/sprite.h"

static int l_update_position(lua_State* L){
    Sprite* sprite = (Sprite*)luaL_checkudata(L, 1, "SpriteMeta");
    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    sprite->update_position(x, y);
    return 0;
}

static int l_update_size(lua_State* L){
    Sprite* sprite = (Sprite*)luaL_checkudata(L, 1, "SpriteMeta");
    float width = luaL_checknumber(L, 2);
    float height = luaL_checknumber(L, 3);
    sprite->update_size(width, height);
    return 0;
}

static int l_scale(lua_State* L){
    Sprite* sprite = (Sprite*)luaL_checkudata(L, 1, "SpriteMeta");
    float scale_x = (float)luaL_optnumber(L, 2, 1.0f);
    float scale_y = (float)luaL_optnumber(L, 3, 1.0f);

    float new_width = sprite->get_width() * scale_x;
    float new_height = sprite->get_height() * scale_y;
    sprite->update_size(new_width, new_height);
    return 0;
}

static int l_update_pivot(lua_State* L){
    Sprite* sprite = (Sprite*)luaL_checkudata(L, 1, "SpriteMeta");
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    sprite->update_pivot(x, y);
    return 0;
}

static int l_update_color(lua_State* L){
    Sprite* sprite = (Sprite*)luaL_checkudata(L, 1, "SpriteMeta");
    Color color = get_color(L, 2);
    sprite->update_color(color);
    return 0;
}

static int l_update_uv(lua_State* L){
    Sprite* sprite = (Sprite*)luaL_checkudata(L, 1, "SpriteMeta");
    UVCoords uv = get_sprite_cut(L, 2);
    sprite->update_uv(uv);
    return 0;
}

static int l_update_texture(lua_State* L){
    Sprite* sprite = (Sprite*)luaL_checkudata(L, 1, "SpriteMeta");
    unsigned int texture = (unsigned int)luaL_checkinteger(L, 2);
    sprite->update_texture(texture);
    return 0;
}

static int l_update_visible(lua_State* L){
    Sprite* sprite = (Sprite*)luaL_checkudata(L, 1, "SpriteMeta");
    bool visible = lua_toboolean(L, 2);
    sprite->update_visibility(visible);
    return 0;
}

static int l_mirror(lua_State* L){
    Sprite* sprite = (Sprite*)luaL_checkudata(L, 1, "SpriteMeta");
    bool horizontal = lua_toboolean(L, 2);
    bool vertical = lua_toboolean(L, 3);
    sprite->mirror(horizontal, vertical);
    return 0;
}

static int l_rotation(lua_State* L){
    Sprite* sprite = (Sprite*)luaL_checkudata(L, 1, "SpriteMeta");
    int degrees = luaL_checknumber(L, 2);
    sprite->update_rotation(degrees);
    return 0;
}

static int l_update(lua_State* L){
    Sprite* sprite = (Sprite*)luaL_checkudata(L, 1, "SpriteMeta");

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float width = luaL_checknumber(L, 4);
    float height = luaL_checknumber(L, 5);

    UVCoords uv = get_sprite_cut(L, 6);
    Color color = get_color(L, 7);
    
    sprite->update(x, y, width, height, uv, color);
    return 0;
}

static int l_draw(lua_State* L) {
    EngineContext* ctx = get_ctx(L);
    Sprite* sprite = (Sprite*)luaL_checkudata(L, 1, "SpriteMeta");
    sprite->draw(ctx->gfx);
    return 0;
}

static int l_destroy(lua_State* L){
    Sprite* sprite = (Sprite*)luaL_checkudata(L, 1, "SpriteMeta");
    sprite->~Sprite();
    return 0;
}

static int sprite_gc(lua_State* L) {
    Sprite* s = (Sprite*)luaL_checkudata(L, 1, "SpriteMeta");
    s->~Sprite();
    return 0;
}

void bind_gfx_sprite(lua_State* L)
{
    static const luaL_Reg gfx_sprite_lib[] = {
        {"pos", l_update_position},
        {"size", l_update_size},
        {"scale", l_scale},
        {"pivot", l_update_pivot},
        {"color", l_update_color},
        {"uv", l_update_uv},
        {"texture", l_update_texture},
        {"visible", l_update_visible},
        {"mirror", l_mirror},
        {"rotation", l_rotation},

        {"update", l_update},
        {"draw", l_draw},

        {"destroy", l_destroy}, // - work in progress
        {NULL, NULL}
    };
    luaL_newlib(L, gfx_sprite_lib);
    lua_setglobal(L, "SPRITE");

    // 2. Setup the Metatable so sprite:draw() works
    luaL_newmetatable(L, "SpriteMeta");
    
    // Point __index to the SPRITE table we just created
    lua_getglobal(L, "SPRITE");
    lua_setfield(L, -2, "__index");

    // Add your GC function from the previous step
    lua_pushcfunction(L, sprite_gc);
    lua_setfield(L, -2, "__gc");

    lua_pop(L, 1); // Clean stack
}
