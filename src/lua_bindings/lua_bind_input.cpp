#include "lua_bindings.h"
#include "engine_context.h"
#include "user_input/user_input.h"

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

void bind_input(lua_State* L){
    static const luaL_Reg input_lib[] = {
    {"btn", l_btn},
    {"btnp", l_btnp},
    {"mpos", l_mpos},
    {NULL, NULL}
    };
    luaL_newlib(L, input_lib);
    lua_setglobal(L, "Input");
}