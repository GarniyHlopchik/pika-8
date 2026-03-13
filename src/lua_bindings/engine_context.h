#pragma once
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

struct EngineContext;

EngineContext* get_ctx(lua_State* L);