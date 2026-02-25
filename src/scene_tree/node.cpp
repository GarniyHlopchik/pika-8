#include "node.h"

Node::Node(Node* p_parent,int p_id,int p_script_ref, LuaSystem* p_L, SceneTree* p_tree){
    parent = p_parent;
    id = p_id;
    if(p_script_ref){
        script_ref = p_script_ref;
    }
    lua = p_L;
    scene_tree = p_tree;
}

void Node::add_child(std::unique_ptr<Node> child) {
    child->parent = this;
    children.push_back(std::move(child));
}
void Node::remove_child(Node* child_ptr) {
    // We search for the unique_ptr that matches the raw pointer address
    children.erase(
        std::remove_if(children.begin(), children.end(),
            [child_ptr](const std::unique_ptr<Node>& ptr) {
                return ptr.get() == child_ptr;
            }),
        children.end()
    );
}
void Node::_init() {
    if (lua && script_ref) {
        lua->call_init(script_ref, id);
    }

    for (auto& child : children) { 
        child->_init();
    }
}
void Node::_update(float dt) {
    for (auto& child : children) {
        child->_update(dt);
    }
}
