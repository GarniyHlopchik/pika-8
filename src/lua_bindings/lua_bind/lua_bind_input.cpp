#include "../lua_bindings.h"
#include "user_input/user_input.h"
#include "user_input/button/button.h"
#include "../../mobile_input/input_state.h"


static int l_btn(lua_State* L){
    EngineContext* ctx = get_ctx(L);
    int key = luaL_checknumber(L,1);
    bool result = IsKeyPressed(*(ctx->input_state), key);
    lua_pushboolean(L,result);
    return 1;
}

static int l_btnp(lua_State* L){
    EngineContext* ctx = get_ctx(L);
    int key = luaL_checknumber(L,1);
    bool result = IsKeyDown(*(ctx->input_state), key);
    lua_pushboolean(L,result);
    return 1;
}

static int l_mpos(lua_State* L){
    EngineContext* ctx = get_ctx(L);
    double* position = getRelativeCursorPosition(*(ctx->input_state));
    lua_pushnumber(L,position[0]);
    lua_pushnumber(L,position[1]);
    delete[] position;
    return 2;
}

static int l_touch_count(lua_State* L) {
    EngineContext* ctx = get_ctx(L);
    if (!ctx || !ctx->gfx) {
        lua_pushinteger(L, 0);
        return 1;

    }
    mobile_input::InputState* state = getTouchState();
    lua_pushinteger(L, state ? state->num_touches : 0);

    return 1;
}

static int l_get_touches(lua_State* L) {
    EngineContext* ctx = get_ctx(L);

    if (!ctx || !ctx->gfx) {
        lua_newtable(L);
        return 1;
    }

    mobile_input::InputState* state = getTouchState();
    if (!state) {
        lua_newtable(L);
        return 1;
    }

    lua_createtable(L, state->touches.size(), 0);
    for (size_t i = 0; i < state->touches.size(); i++) {
        auto& t = state->touches[i];
        lua_createtable(L, 0, 8);  

        lua_pushstring(L, "x"); lua_pushnumber(L, t.x); lua_settable(L, -3);
        lua_pushstring(L, "y"); lua_pushnumber(L, t.y); lua_settable(L, -3);
        lua_pushstring(L, "dx"); lua_pushnumber(L, t.dx); lua_settable(L, -3);
        lua_pushstring(L, "dy"); lua_pushnumber(L, t.dy); lua_settable(L, -3);
        lua_pushstring(L, "down"); lua_pushboolean(L, t.down); lua_settable(L, -3);

        
        lua_pushstring(L, "just_pressed"); lua_pushboolean(L, t.just_pressed); lua_settable(L, -3);
        lua_pushstring(L, "just_released"); lua_pushboolean(L, t.just_released); lua_settable(L, -3);
        lua_pushstring(L, "button"); lua_pushinteger(L, t.button); lua_settable(L, -3);
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}

static int l_get_position(lua_State* L) {
    EngineContext* ctx = get_ctx(L);
    if (!ctx || !ctx->gfx) {
        lua_pushnumber(L, 0);
        lua_pushnumber(L, 0);
        lua_pushboolean(L, false);
        return 3;
    }

    mobile_input::InputState* state = getTouchState();
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

static int l_is_down(lua_State* L) {
    EngineContext* ctx = get_ctx(L);
    if (!ctx || !ctx->gfx) {
        lua_pushboolean(L, false);
        return 1;
    }

    mobile_input::InputState* state = getTouchState();
    lua_pushboolean(L, state ? state->num_touches > 0 : false);
    return 1;
}

static int l_get_swipe(lua_State* L) {
    EngineContext* ctx = get_ctx(L);
    if (!ctx || !ctx->gfx) {
        lua_pushnumber(L, 0);
        lua_pushnumber(L, 0);
        return 2;
    }

    mobile_input::InputState* state = getTouchState();
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

static int l_is_emulating(lua_State* L) {
    EngineContext* ctx = get_ctx(L);
    if (!ctx || !ctx->gfx) {
        lua_pushboolean(L, false);
        return 1;
    }

    mobile_input::InputState* state = getTouchState();
    lua_pushboolean(L, state ? state->is_emulating : false);
    return 1;
}



/// BUTTON
static int l_create_button(lua_State* L) {
	EngineContext* ctx = get_ctx(L);
    std::string name = luaL_checkstring(L, 1);
    Vec2 pos = get_vec2(L, 2);
    Vec2 size = get_vec2(L, 3);
    unsigned int id = luaL_checkinteger(L, 4);
    unsigned int texture = lua_tonumber(L, 5);

    unsigned int button_id = button::create_button(pos.first, pos.second, size.first, size.second, id, texture);
    lua_pushinteger(L, button_id);

    return 1;
}

static int l_button_set_on_press_callback(lua_State* L) {
    unsigned int button_id = luaL_checkinteger(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    
    sol::state_view sv(L);
    sol::function callback(sv, sol::raw_index(2));
    button::button_set_on_press_callback(button_id, callback);
    
    return 0;
}

static int l_button_set_on_hold_callback(lua_State* L) {
    unsigned int button_id = luaL_checkinteger(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    
    sol::state_view sv(L);
    sol::function callback(sv, sol::raw_index(2));
    button::button_set_on_hold_callback(button_id, callback);
    
    return 0;
}

static int l_button_draw(lua_State* L) {
	EngineContext* ctx = get_ctx(L);
	unsigned int button_id = luaL_checkinteger(L, 1);
	
	button::button_draw(button_id, ctx->gfx);
	return 0;
}

static int l_button_draw_all(lua_State* L) {
	EngineContext* ctx = get_ctx(L);
	button::button_draw_all(ctx->gfx);
	return 0;
}

static int l_button_update_all(lua_State* L) {
	EngineContext* ctx = get_ctx(L);
	button::button_update_all(*(ctx->input_state));
	return 0;
}

static int l_button_destroy(lua_State* L) {
	unsigned int button_id = luaL_checkinteger(L, 1);
	button::button_destroy(button_id);
	return 0;
}


void bind_input(lua_State* L) {
    static const luaL_Reg input_lib[] = {
    
        {"btn", l_btn},
        {"btnp", l_btnp},
        {"mpos", l_mpos},

    
        {"touch_count", l_touch_count},
        {"get_touches", l_get_touches},
        {"get_position", l_get_position},
        {"is_down", l_is_down},
        {"get_swipe", l_get_swipe},
        {"is_emulating", l_is_emulating},

        {"create_button", l_create_button},
        {"button_set_on_press_callback", l_button_set_on_press_callback},
        {"button_set_on_hold_callback", l_button_set_on_hold_callback},
        {"button_draw", l_button_draw},
        {"button_draw_all", l_button_draw_all},
        {"button_update_all", l_button_update_all},
        {"button_destroy", l_button_destroy},

        {NULL, NULL}
    };
    luaL_newlib(L, input_lib);
    lua_setglobal(L, "Input");
}