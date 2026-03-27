#pragma once
#include "node.h"

struct Vector2{
    float x;
    float y;
};
class Node2D : public Node {
    public:
    Node2D(int p_id, LuaSystem* p_L, SceneTree* p_tree, float x, float y) : Node(p_id,p_L,p_tree), position({x,y}) {};
    Vector2 get_global_position();
    void push_to_lua(lua_State* L) override {
        sol::stack::push(L, std::ref(*this));
    }
    protected:
    Vector2 position;
};