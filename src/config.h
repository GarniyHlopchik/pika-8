#pragma once
#ifdef __ANDROID__
    #include <GLES3/gl3.h>
    #include <GLES3/gl3ext.h>
#else
    #include <glad/glad.h>
#endif
#include "iostream"
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
    #include "luasocket.h" 
    
    // The entry point for the C library
    int luaopen_socket_core(lua_State *L);
    int luaopen_mime_core(lua_State *L);

    #include "miniz/miniz.h"
}

#include "fstream"
#include "sstream"
#include "string"
#include "vector"
#include "cstdint"
#include "tuple"
#include "array"
#include "chrono"
#include "algorithm"
#include "list"
#include "iterator"
#include "pugixml/pugixml.hpp"
#include "memory"
#include "utility"
#include "lua_system/lua_system.h"
#include "scene_tree/scene_tree.h"
#include "scene_tree/node.h"
#include "sol/sol.hpp"
#include "globals.h"
