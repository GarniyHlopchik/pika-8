#include "../lua_bindings.h"
#include "scene_tree/scene_tree.h"
#include "scene_tree/node_factory.h"
#include "sol/sol.hpp"


static int l_create(lua_State* L){
    EngineContext* ctx = get_ctx(L);
    std::string type = luaL_checkstring(L,1);
    std::string script_path = luaL_optstring(L,2,"");
    std::unique_ptr<Node> node = NodeFactory::create(type,ctx->lua,ctx->scene_tree,script_path);
    sol::stack::push(L, std::move(node));
    return 1;
}

void bind_node(lua_State* L){
    static const luaL_Reg node_lib[] = {
    {"create", l_create},
    {NULL, NULL}
    };
    luaL_newlib(L, node_lib);
    lua_setglobal(L, "Node");
}