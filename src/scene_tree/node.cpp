#include "node.h"

Node::Node(Node* p_parent,int p_id){
    parent = p_parent;
    id = p_id;
}
virtual Node::~Node(){
    for(Node* child : children){
        remove_child(child);
    }
}
void add_child(Node* child){
    children.push_back(child);
}
void remove_child(Node* child){
    auto new_end = std::remove(children.begin(), children.end(), child);
    children.erase(new_end, children.end());
}