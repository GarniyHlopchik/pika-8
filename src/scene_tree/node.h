#pragma once
#include "config.h"
#include "sol/sol.hpp"
#include <climits>
class SceneTree;
class LuaSystem;
class Node{
public:
    Node(int p_id, LuaSystem* p_L, SceneTree* p_tree) : id(p_id), lua(p_L), scene_tree(p_tree), sort_index(UINT_MAX) {};
    virtual ~Node();

    void add_child(std::unique_ptr<Node> child);
    void remove_child(Node* child);

    virtual void _update(float dt);
    virtual void _init(int script_ref);

    unsigned int get_sort_index() const { return sort_index; }
    void set_sort_index(unsigned int index) { sort_index = index; }

    virtual void push_to_lua(lua_State* L) {
        sol::stack::push(L, std::ref(*this));
    }


    Node* parent;
    std::vector<std::unique_ptr<Node>> children;
    unsigned int sort_index;
    
    LuaSystem* lua;
    SceneTree* scene_tree;

    int script_ref;
    int id;

};