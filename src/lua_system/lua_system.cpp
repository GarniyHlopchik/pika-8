#include "lua_system.h"

LuaSystem::LuaSystem(){
    //lua setup-----------------------------
    L = luaL_newstate();  // Lua vm
    luaL_openlibs(L);               // setup

    // Preload the "socket.core" module
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    
    lua_pushcfunction(L, luaopen_socket_core);
    lua_setfield(L, -2, "socket.core");

    // Register mime.core
    lua_pushcfunction(L, luaopen_mime_core);
    lua_setfield(L, -2, "mime.core");
    
    lua_pop(L, 2);

    const char* path_logic = 
        "local app_path = './lua_modules/?.lua;./lua_modules/?/init.lua;';"
        "local game_path = './?.lua;./?/init.lua;';"
        "package.path = app_path .. game_path .. package.path";

    if (luaL_dostring(L, path_logic) != LUA_OK) {
        std::cout << "Issue setting up lua_modules path" << std::endl;
    }
    
}
LuaSystem::~LuaSystem(){
    lua_close(L);
    for(int t : tables){
        luaL_unref(L, LUA_REGISTRYINDEX, t);
    }
}
void LuaSystem::load_script(const std::string& path){
    if (luaL_dofile(L, path.c_str()) != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        std::cout << "Issue loading " << path << std::endl;
        std::cout << err << std::endl;
    }
}
int LuaSystem::load_script_table(const std::string& path) {
    // Load + run script
    if (luaL_dofile(L, path.c_str()) != LUA_OK) {
        std::cerr << "Lua error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
        return LUA_REFNIL;
    }

    // Ensure it returned a table
    if (!lua_istable(L, -1)) {
        std::cerr << "Lua script did not return a table!" << std::endl;
        lua_pop(L, 1);
        return LUA_REFNIL;
    }

    // Store table in registry
    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    tables.push_back(ref);
    return ref;
}
void LuaSystem::remove_table(int id){
    // Find the iterator pointing to the first occurrence of the value
    auto it = std::find(tables.begin(), tables.end(), id);

    // If the value is found, erase it
    if (it != tables.end()) {
        tables.erase(it);
    }
}
void LuaSystem::call_init(int table_ref, int node_id) {
    // Push table (self)
    lua_rawgeti(L, LUA_REGISTRYINDEX, table_ref);

    if (!lua_istable(L, -1)) {
        std::cerr << "Invalid Lua table reference!" << std::endl;
        lua_pop(L, 1);
        return;
    }

    // Get _init function
    lua_getfield(L, -1, "_init");

    if (!lua_isfunction(L, -1)) {
        // No _init defined — not an error
        lua_pop(L, 2); // function + table
        return;
    }

    // Stack now:
    // [ table ][ function ]

    // Push self
    lua_pushvalue(L, -2); // table

    // Push node_id
    lua_pushinteger(L, node_id);

    // Call function(self, node_id)
    if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
        std::cerr << "Lua _init error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }

    // Pop table
    lua_pop(L, 1);
}
void LuaSystem::call_script_update(float dt)
{
    for (int table_ref : tables)
    {
        // Push table onto stack (self)
        lua_rawgeti(L, LUA_REGISTRYINDEX, table_ref);

        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);
            continue;
        }

        // Get table._update
        lua_getfield(L, -1, "_update");

        if (!lua_isfunction(L, -1)) {
            // No update method → pop function + table
            lua_pop(L, 2);
            continue;
        }

        // Push self
        lua_pushvalue(L, -2);

        // Push dt
        lua_pushnumber(L, dt);

        // Call _update(self, dt)
        if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
            std::cerr << "Lua _update error: "
                      << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1); // error message
        }

        // Pop table
        lua_pop(L, 1);
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