#pragma once
#include "config.h"

class Node{
    public:
    Node(Node* p_parent,int p_id);
    virtual ~Node();
    void add_child(Node* child);
    void remove_child(Node* child);
    virtual void _init();
    virtual void _update(float dt);
    private:
    Node* parent;
    std::vector<Node*> children;
    int id;
};