#include "config.h"

void sol_bind_node(lua_State* L);
void sol_bind_sprite(lua_State* L);
void bind_nodes(lua_State* L){
    sol_bind_node(L);
    sol_bind_sprite(L);
}