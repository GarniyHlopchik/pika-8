#include "config.h"
#include "../lua_bindings.h"
#include "scene_tree/node2d.h"
void sol_bind_node2d(lua_State* L){
    
    sol::state_view lua(L);
    lua.new_usertype<Node2D>("Node2D",
        sol::base_classes, sol::bases<Node>(),
        "get_global_position", [](Node2D& self, sol::this_state ts){
            sol::state_view lua(ts);
            Vector2 pos =  self.get_global_position();
            sol::table output = lua.create_table();
            output[1]=pos.x;
            output[2]=pos.y;
            return output;
        }
    );
}