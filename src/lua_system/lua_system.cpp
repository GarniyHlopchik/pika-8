#include "lua_system.h"
#include "file_resolve/file_system.h"
#include <iostream>
#include "gfx/sprite/sprite.h"
#include "lua_bindings/sol_bind/sol.h"
#include "lua_bindings/lua_bindings.h"

extern "C" int luaopen_Input(lua_State* L);
int lua_zip_searcher(lua_State* L) {
    const char* module = luaL_checkstring(L, 1);
    
    std::string path = module;
    std::replace(path.begin(), path.end(), '.', '/');
    path += ".lua";

    Resource res = FileSystem::get_resource(path);

    if (!res.is_valid()) {
        lua_pushfstring(L, "\n\tno file '%s' in zip", path.c_str());
        return 1; 
    }

    // Use the path for the chunk name (for debug traces)
    int status = luaL_loadbuffer(L, 
                                (const char*)res.data.get(), 
                                res.size, 
                                ("@" + path).c_str());

    if (status != LUA_OK) {
        // Let Lua handle the syntax error
        return lua_error(L); 
    }

    // SUCCESS: 
    // Return 1: The compiled function (already on stack from loadbuffer)
    // Return 2: The ORIGINAL module name (with dots). This is what Lua expects!
    lua_pushstring(L, module); 
    return 2;
}
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
    //sol setup
    bind_nodes(L);
    lua_newtable(L);

    // touch_count
    lua_pushstring(L, "touch_count");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_pushinteger(L, 0);
        return 1;
        });
    lua_settable(L, -3);

    // get_position
    lua_pushstring(L, "get_position");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_pushnumber(L, 0);
        lua_pushnumber(L, 0);
        lua_pushboolean(L, false);
        return 3;
        });
    lua_settable(L, -3);

    // get_touches
    lua_pushstring(L, "get_touches");
    lua_pushcfunction(L, [](lua_State* L) -> int {
        lua_newtable(L);
        return 1;
        });
    lua_settable(L, -3);

    lua_setglobal(L, "Input");

    //allows lua to seek for require in zip instead of directory
    // 1. Get package.searchers onto the stack
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "searchers");    // Stack: [package, searchers]

    // 2. Get table.insert onto the stack
    lua_getglobal(L, "table");
    lua_getfield(L, -1, "insert");       // Stack: [package, searchers, table, insert]

    // 3. Prepare arguments for table.insert(searchers, 2, lua_zip_searcher)
    lua_pushvalue(L, -3);                // Push searchers table (Arg 1)
    lua_pushinteger(L, 2);               // Push index 2 (Arg 2)
    lua_pushcfunction(L, lua_zip_searcher); // Push function (Arg 3)

    // 4. Call table.insert(searchers, 2, func)
    if (lua_pcall(L, 3, 0, 0) != LUA_OK) {
        std::cout << "Error injecting searcher: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }

    // 5. Clean up the stack (pop 'table', 'searchers', and 'package')
    lua_pop(L, 3);
    bind_input(L);
}
LuaSystem::~LuaSystem(){
    lua_close(L);
    for(int t : tables){
        luaL_unref(L, LUA_REGISTRYINDEX, t);
    }
}

lua_State* LuaSystem::get_state(){
    return L;
}
void LuaSystem::set_context(EngineContext* ctx) {
    lua_pushlightuserdata(L, ctx);
    lua_setfield(L, LUA_REGISTRYINDEX, "engine_ctx");
}

void LuaSystem::load_script(const std::string& path){
    Resource res = FileSystem::get_resource(path);

    if(!res.is_valid()){
        std::cout << "Issue loading " << path << std::endl;
        return;
    }

    if(luaL_loadbuffer(
        L,
        (const char*)res.data.get(),
        res.size,
        path.c_str()
    ) != LUA_OK){
        std::cout << "Lua compile error: " << lua_tostring(L,-1) << std::endl;
        lua_pop(L,1);
        return;
    }

    if(lua_pcall(L,0,0,0) != LUA_OK){
        std::cout << "Lua runtime error: " << lua_tostring(L,-1) << std::endl;
        lua_pop(L,1);
    }
}
int LuaSystem::load_script_table(const std::string& path){
    Resource res = FileSystem::get_resource(path);

    if(!res.is_valid()){
        std::cerr << "Failed to load script: " << path << std::endl;
        return LUA_REFNIL;
    }

    if(luaL_loadbuffer(
        L,
        (const char*)res.data.get(),
        res.size,
        path.c_str()
    ) != LUA_OK){
        std::cerr << "Lua compile error: " << lua_tostring(L,-1) << std::endl;
        lua_pop(L,1);
        return LUA_REFNIL;
    }

    if(lua_pcall(L,0,1,0) != LUA_OK){
        std::cerr << "Lua runtime error: " << lua_tostring(L,-1) << std::endl;
        lua_pop(L,1);
        return LUA_REFNIL;
    }

    if(!lua_istable(L,-1)){
        std::cerr << "Lua script did not return a table!" << std::endl;
        lua_pop(L,1);
        return LUA_REFNIL;
    }

    int ref = luaL_ref(L, LUA_REGISTRYINDEX);
    tables.push_back(ref);

    std::cout << "Loaded script, ref = " << ref << std::endl;

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
void LuaSystem::call_init(int table_ref, Node& node_ref) {
    // 1. Get the table from registry
    lua_rawgeti(L, LUA_REGISTRYINDEX, table_ref);

    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return;
    }

    // 2. Get the _init function from the table
    lua_getfield(L, -1, "_init");

    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 2); 
        return;
    }

    // 3. Push 'self' (the table)
    lua_pushvalue(L, -2); 

    // 4. Push the Node reference using sol2
    // This uses the bindings you already defined for the Node class
    node_ref.push_to_lua(L);

    // Stack is now: [table] [function] [self] [node_userdata]
    
    // 5. Call function(self, node)
    if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
        std::cerr << "Lua _init error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }

    // 6. Pop the original table
    lua_pop(L, 1);
}
void LuaSystem::call_script_update(int table_ref, float dt)
{
        // Push table onto stack (self)
        lua_rawgeti(L, LUA_REGISTRYINDEX, table_ref);

        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);
            return;
        }

        // Get table._update
        lua_getfield(L, -1, "_update");

        if (!lua_isfunction(L, -1)) {
            // No update method → pop function + table
            lua_pop(L, 2);
            return;
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
        std::cout << "UPDATE LOOP ERROR" << err << std::endl;
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
        std::cout << "ERROR IN CALLBACK " << name << ": "<< err << std::endl;
        lua_pop(L, 1);
    }
}
void LuaSystem::bind_lib(const luaL_Reg* lib, const std::string& name) {
    lua_newtable(L);             
    luaL_setfuncs(L, lib, 0);     
    lua_setglobal(L, name.c_str());
}
