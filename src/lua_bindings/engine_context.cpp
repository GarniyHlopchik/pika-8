#include "engine_context.h"
EngineContext* get_ctx(lua_State* L) {
    lua_getfield(L, LUA_REGISTRYINDEX, "engine_ctx");
    EngineContext* ctx =
        (EngineContext*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    return ctx;
}