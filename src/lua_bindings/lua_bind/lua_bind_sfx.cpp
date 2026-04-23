#include "../lua_bindings.h"
#include "sfx/sfx.h"



//// SFX Lua bindings
static int l_sfx_load(lua_State* L){
    EngineContext* ctx = get_ctx(L);
    const char* path = luaL_checkstring(L,1);

    // 1. Create a "Proxy" table: { ready = false }
    lua_newtable(L);
    lua_pushboolean(L, 0);
    lua_setfield(L, -2, "ready");
    
    // Create a "strong reference" so we can find this table later
    lua_pushvalue(L, -1); // Duplicate the table on stack
    int registry_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    ctx->sfx->schedule_load(path,registry_ref);
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
static int l_sfx_play(lua_State* L){
    EngineContext* ctx = get_ctx(L);
    unsigned int id = luaL_checknumber(L,1);
    float volume = luaL_optnumber(L, 2, 100.0f);
    float pitch = luaL_optnumber(L, 3, 1.0f);
    bool loop = lua_toboolean(L, 4);    
    float pan = luaL_optnumber(L, 7, 0.0f);
    ctx->sfx->play(id, volume, pitch, loop, pan);
    return 0;
}

static int l_sfx_stop(lua_State* L){
    EngineContext* ctx = get_ctx(L);
    unsigned int id = luaL_checknumber(L,1);
    ctx->sfx->stop(id);
    return 0;
}

void bind_sfx(lua_State* L){
    static const luaL_Reg sfx_lib[] = {
    {"load",l_sfx_load},
    {"play",l_sfx_play},
    {"stop",l_sfx_stop},
    {NULL,NULL}
    };
    luaL_newlib(L, sfx_lib);
    lua_setglobal(L, "SFX");
}