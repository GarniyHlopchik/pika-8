#pragma once
#include "config.h"
class SceneTree;
class LuaSystem;
class Node{
    public:
    Node(Node* p_parent, int p_id, int p_script_ref, LuaSystem* p_L, SceneTree* p_tree);
    virtual ~Node() = default;
    void add_child(std::unique_ptr<Node> child);
    void remove_child(Node* child);
    virtual void _init();
    virtual void _update(float dt);

    private:
    Node* parent;
    std::vector<std::unique_ptr<Node>> children;
    int id;
    int script_ref;
    LuaSystem* lua; 
    SceneTree* scene_tree;
};