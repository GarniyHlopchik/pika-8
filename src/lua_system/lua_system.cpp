#include "lua_system.h"

LuaSystem::LuaSystem(){
    //lua setup-----------------------------
    L = luaL_newstate();  // Lua vm
    luaL_openlibs(L);               // setup
    
}
LuaSystem::~LuaSystem(){
    lua_close(L);
}
void LuaSystem::load_script(const std::string& path){
    if (luaL_dofile(L, path.c_str()) != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        std::cout << "Issue loading " << path << std::endl;
        std::cout << err << std::endl;
    }
}
void LuaSystem::call_update(float dt){
    lua_getglobal(L, "_update");

    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }

    // Push arguments (in order)
    lua_pushnumber(L, dt);

    // Call: 1 argument, 0 return values
    if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        //log_error(err);   WRITE ERROR LOGGING LATER
        lua_pop(L, 1);
    }
}
void LuaSystem::call(const char* name){
    lua_getglobal(L, name);
    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        //log_error(err);
        lua_pop(L, 1);
    }
}
void LuaSystem::bind_lib(const luaL_Reg* lib, const std::string& name) {
    lua_newtable(L);             
    luaL_setfuncs(L, lib, 0);     
    lua_setglobal(L, name.c_str());
}