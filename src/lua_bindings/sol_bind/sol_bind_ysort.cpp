#include "config.h"
#include "../lua_bindings.h"
#include "scene_tree/node2d.h"
#include "scene_tree/y_sort.h"
void sol_bind_ysort(lua_State* L){
    
    sol::state_view lua(L);
    lua.new_usertype<YSort>("YSort",
        sol::base_classes, sol::bases<Node2D>()
    );
}