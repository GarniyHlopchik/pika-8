#pragma once

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

void bind_gfx(lua_State* L);
void bind_input(lua_State* L);
void bind_sfx(lua_State* L);
void bind_node(lua_State* L);