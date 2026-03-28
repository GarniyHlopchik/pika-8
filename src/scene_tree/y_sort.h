#pragma once
#include "node2d.h"
#include "node.h"
#include <algorithm>
class YSort : public Node2D{
    public:
    YSort(int p_id, LuaSystem* p_L, SceneTree* p_tree,float x, float y) : Node2D(p_id,p_L,p_tree,x,y) {};
    void _update(float delta) override;
    void push_to_lua(lua_State* L) override {
        sol::stack::push(L, std::ref(*this));
    }

};