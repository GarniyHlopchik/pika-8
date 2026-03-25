#include "../lua_bindings.h"
#include "scene_tree/scene_tree.h"
#include "scene_tree/node_factory.h"
#include "sol/sol.hpp"
#include "gfx/sprite/sprite.h"


static int l_create(lua_State* L){
    EngineContext* ctx = get_ctx(L);
    std::string type = luaL_checkstring(L,1);
    std::string script_path = luaL_optstring(L,2,"");
    
    if(type=="node"){
            std::unique_ptr<Node> node = NodeFactory::create_node(ctx->lua,ctx->scene_tree,script_path);
            sol::stack::push(L, std::move(node));
    }
    else if(type=="sprite"){
        
            unsigned int texture = (unsigned int)luaL_checkinteger(L,3);
            float x = luaL_optnumber(L,4,0);
            float y = luaL_optnumber(L,5,0);
            float width = luaL_optnumber(L,6,8.0);
            float height = luaL_optnumber(L,7,8.0);
            UVCoords uv;
            if (lua_isnoneornil(L, 8)) {
                uv.u1 = 0.0;
                uv.v1 = 0.0;
                uv.u2 = 1.0;
                uv.v2 = 1.0;
            }
            else{
                uv = get_sprite_cut(L,8);
            }
            Color color;
            if (lua_isnoneornil(L, 9)) {
                color.r = 1.0;
                color.g = 1.0;
                color.b = 1.0;
                color.a = 1.0;
            }
            else{
                color = get_color(L,9);
            }
            std::unique_ptr<Sprite> node = NodeFactory::create_sprite(ctx->lua,ctx->scene_tree,script_path,texture,x,y,width,height,uv,color);
            sol::stack::push(L, std::move(node));
    }
        
    
    
    
    return 1;
}

void bind_node(lua_State* L){
    static const luaL_Reg node_lib[] = {
    {"create", l_create},
    {NULL, NULL}
    };
    luaL_newlib(L, node_lib);
    lua_setglobal(L, "NodeFactory");
}