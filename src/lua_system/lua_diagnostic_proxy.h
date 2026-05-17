#pragma once

#include "lua_system_interface.h"
#include "lua_system.h"
class LuaDiagnosticProxy : public LuaSystemInterface {
    public:
        LuaDiagnosticProxy(LuaSystem& lua) : real_lua(lua){};
        void call(const char* name);
        void call_update(float dt);
        void call_init(int table_ref, Node& node);
        void call_script_update(int table_ref,float dt);
    private:
        LuaSystem& real_lua;
};