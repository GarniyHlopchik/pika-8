#pragma once
#include "glad/glad.h"
#include "iostream"
#include "GLFW/glfw3.h"
extern "C" {
    #include "lua.h"
    #include "lauxlib.h"
    #include "lualib.h"
    #include "luasocket.h" 
    
    // The entry point for the C library
    int luaopen_socket_core(lua_State *L);
    int luaopen_mime_core(lua_State *L);
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