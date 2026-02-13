#pragma once
#include "config.h"

class LuaSystem{
    public:
    LuaSystem();
    ~LuaSystem();
    void load_script(const std::string& path);
    void call_update(float dt);
    void call(const char* name);
    void bind_lib(const luaL_Reg* lib, const std::string& name);
    private:
    lua_State* L;
};