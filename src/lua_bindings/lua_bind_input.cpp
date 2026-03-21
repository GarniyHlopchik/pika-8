#include "lua.hpp"
#include "../../gfx/gfx.h"
#include "../../mobile_input/input_state.h"

static mobile_input::InputState* get_state(lua_State* L) {
    lua_getglobal(L, "gfx");
    if (!lua_islightuserdata(L, -1)) {
        lua_pop(L, 1);
        return nullptr;
    }
    GFX* gfx = (GFX*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    return gfx ? gfx->getMobileInputState() : nullptr;
}

static int lua_get_touches(lua_State* L) {
    auto* state = get_state(L);
    if (!state) { lua_newtable(L); return 1; }

    lua_createtable(L, state->touches.size(), 0);
    for (size_t i = 0; i < state->touches.size(); i++) {
        auto& t = state->touches[i];
        lua_createtable(L, 0, 5);

        lua_pushstring(L, "x"); lua_pushnumber(L, t.x); lua_settable(L, -3);
        lua_pushstring(L, "y"); lua_pushnumber(L, t.y); lua_settable(L, -3);
        lua_pushstring(L, "dx"); lua_pushnumber(L, t.dx); lua_settable(L, -3);
        lua_pushstring(L, "dy"); lua_pushnumber(L, t.dy); lua_settable(L, -3);
        lua_pushstring(L, "down"); lua_pushboolean(L, t.down); lua_settable(L, -3);

        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}

static int lua_touch_count(lua_State* L) {
    auto* state = get_state(L);
    lua_pushinteger(L, state ? state->num_touches : 0);
    return 1;
}

static int lua_is_down(lua_State* L) {
    auto* state = get_state(L);
    lua_pushboolean(L, state ? state->num_touches > 0 : false);
    return 1;
}

static int lua_get_position(lua_State* L) {
    auto* state = get_state(L);
    if (state && !state->touches.empty()) {
        auto& t = state->touches[0];
        lua_pushnumber(L, t.x);
        lua_pushnumber(L, t.y);
        lua_pushboolean(L, t.down);
        return 3;
    }
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushboolean(L, false);
    return 3;
}

static int lua_get_swipe(lua_State* L) {
    auto* state = get_state(L);
    if (state && !state->touches.empty()) {
        auto& t = state->touches[0];
        lua_pushnumber(L, t.dx);
        lua_pushnumber(L, t.dy);
        return 2;
    }
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    return 2;
}

static const luaL_Reg funcs[] = {
    {"get_touches", lua_get_touches},
    {"touch_count", lua_touch_count},
    {"is_down", lua_is_down},
    {"get_position", lua_get_position},
    {"get_swipe", lua_get_swipe},
    {NULL, NULL}
};

extern "C" int luaopen_Input(lua_State* L) {
    luaL_newlib(L, funcs);
    return 1;
}