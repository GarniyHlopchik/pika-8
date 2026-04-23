#pragma once
#include "config.h"
#include "scene_tree/node.h"
struct EngineContext;
class Node;
class GFX;
class LuaSystem{
    public:
    LuaSystem();
    ~LuaSystem();
    lua_State* get_state();
    void set_context(EngineContext* ctx);
    void load_script(const std::string& path);
    int load_script_table(const std::string& path);
    void call_init(int table_ref, Node& node);
    void call_script_update(int table_ref,float dt);
    void call_update(float dt);
    void call(const char* name);
    void bind_lib(const luaL_Reg* lib, const std::string& name);
    void remove_table(int id);
    void resolve_promise(int handle, int table);
    private:
    lua_State* L;
    std::vector<int> tables;
};