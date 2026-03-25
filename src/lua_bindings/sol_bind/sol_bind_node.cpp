#include "config.h"
#include <functional>
void sol_bind_node(lua_State* L){
    sol::state_view lua(L);
    lua.new_usertype<Node>("Node",
    "_update", &Node::_update,
    "add_child", [](Node& self, sol::stack_object child_obj) {
        auto& lua_uptr = child_obj.as<std::unique_ptr<Node>&>();
        if (!lua_uptr) {
            luaL_error(child_obj.lua_state(), "Pika-8 Error: Node is null or already owned");
            return;
        }
        std::unique_ptr<Node> child_ptr = std::move(lua_uptr);
        self.add_child(std::move(child_ptr));
    },
    // Fix: Use lambdas for member variables to satisfy Android Clang
    "id", sol::property([](Node& n) { return n.id; }),
        
    "parent", sol::property([](Node& n) { return n.parent; }),
        
    "children", sol::property([](Node& n) { 
            // Returning as a reference so Lua can iterate over the actual vector
        return std::ref(n.children); 
    }),
    "remove_child", &Node::remove_child,
    
    "script", sol::property([](Node& self, sol::this_state s) -> sol::object {
        if (self.script_ref == LUA_REFNIL) {
            return sol::make_object(s, sol::lua_nil);
        }
        lua_rawgeti(s, LUA_REGISTRYINDEX, self.script_ref);
        return sol::stack::pop<sol::object>(s);
    })
    );
}