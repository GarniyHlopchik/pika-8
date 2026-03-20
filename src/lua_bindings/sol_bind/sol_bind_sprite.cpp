#include "config.h"
#include "../lua_bindings.h"
#include "gfx/sprite/sprite.h"
void sol_bind_sprite(lua_State* L){
    sol::state_view lua(L);
    lua.new_usertype<Sprite>("Sprite",
        sol::base_classes, sol::bases<Node>()
    );


}