#include "node.h"

Node::Node(int p_id, LuaSystem* p_L, SceneTree* p_tree){
    id = p_id;
    lua = p_L;
    scene_tree = p_tree;

    if (lua && script_ref) {
        lua->call_init(script_ref, id);
    }

    
}
Node::~Node(){
    lua->remove_table(script_ref);
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

void Node::_update(float dt) {
    for (auto& child : children) {
        child->_update(dt);
    }
}
