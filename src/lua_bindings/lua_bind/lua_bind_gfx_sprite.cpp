#include "../lua_bindings.h"
#include "../../gfx/sprite/sprite.h"

static int l_update_position(lua_State* L){
    Sprite* sprite = (Sprite*)lua_touserdata(L, 1);
    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    sprite->update_position(x, y);
    return 0;
}

static int l_update_size(lua_State* L){
    Sprite* sprite = (Sprite*)lua_touserdata(L, 1);
    float width = luaL_checknumber(L, 2);
    float height = luaL_checknumber(L, 3);
    sprite->update_size(width, height);
    return 0;
}

static int l_update_color(lua_State* L){
    Sprite* sprite = (Sprite*)lua_touserdata(L, 1);
    Color color = get_color(L, 2);
    sprite->update_color(color);
    return 0;
}

static int l_update_uv(lua_State* L){
    Sprite* sprite = (Sprite*)lua_touserdata(L, 1);
    UVCoords uv = get_sprite_cut(L, 2);
    sprite->update_uv(uv);
    return 0;
}

static int l_update_texture(lua_State* L){
    Sprite* sprite = (Sprite*)lua_touserdata(L, 1);
    unsigned int texture = (unsigned int)luaL_checkinteger(L, 2);
    sprite->update_texture(texture);
    return 0;
}

static int l_update(lua_State* L){
    Sprite* sprite = (Sprite*)lua_touserdata(L, 1);
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

static int sprite_gc(lua_State* L) {
    Sprite* s = (Sprite*)lua_touserdata(L, 1);
    s->~Sprite();
    return 0;
}

void bind_gfx_sprite(lua_State* L)
{
    static const luaL_Reg gfx_sprite_lib[] = {
        {"pos", l_update_position},
        {"size", l_update_size},
        {"color", l_update_color},
        {"uv", l_update_uv},
        {"texture", l_update_texture},

        {"update", l_update},
        {"draw", l_draw},
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
