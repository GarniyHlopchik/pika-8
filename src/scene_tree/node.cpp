#include "node.h"

Node::Node(Node* p_parent,int p_id){
    parent = p_parent;
    id = p_id;
}
Node::~Node(){
    for (std::vector<Node*>::iterator it = children.begin(); it != children.end(); ++it) {
        remove_child(*it);
    }
}
void Node::add_child(Node* child){
    children.push_back(child);
}
void Node::remove_child(Node* child){
    auto new_end = std::remove(children.begin(), children.end(), child);
    children.erase(new_end, children.end());
}
void Node::_init(){}
void Node::_update(float dt){}