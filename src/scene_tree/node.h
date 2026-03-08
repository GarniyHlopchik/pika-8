#pragma once
#include "config.h"
class SceneTree;
class LuaSystem;
class Node{
    public:
    Node(int p_id, LuaSystem* p_L, SceneTree* p_tree);
    virtual ~Node();
    void add_child(std::unique_ptr<Node> child);
    void remove_child(Node* child);
    virtual void _update(float dt);
    void _init(int script_ref);
    int script_ref;
    int id;

    Node* parent;
    std::vector<std::unique_ptr<Node>> children;
    
    
    LuaSystem* lua; 
    SceneTree* scene_tree;
};