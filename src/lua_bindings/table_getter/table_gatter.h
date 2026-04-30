#pragma once
#include "vector"
#include "../../gfx/sprite/sprite.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

std::vector<float> get_table_floats(lua_State* L, int index, int count, float fallback = 0.0f);
std::vector<int> get_table_ints(lua_State* L, int index, int count, int fallback = 0);

Color get_color(lua_State* L, int index, float default_color = 255.0f);

UVCoords get_sprite_cut(lua_State* L, int index);

std::pair<float, float> get_vec2(lua_State* L, int index);

