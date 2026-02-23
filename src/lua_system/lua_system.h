#pragma once
#include "config.h"

class LuaSystem{
    public:
    LuaSystem();
    ~LuaSystem();
    void load_script(const std::string& path);
    int load_script_table(const std::string& path);
    void call_init(int table_ref, int node_id);
    void call_script_update(float dt);
    void call_update(float dt);
    void call(const char* name);
    void bind_lib(const luaL_Reg* lib, const std::string& name);
    void remove_table(int id);
    private:
    lua_State* L;
    std::vector<int> tables;
};