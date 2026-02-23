#pragma once
#include "config.h"

class Node{
    public:
    Node(Node* p_parent,int p_id,int p_script_ref);
    virtual ~Node();
    void add_child(Node* child);
    void remove_child(Node* child);
    void queue_free();
    virtual void _init();
    virtual void _update(float dt);

    private:
    Node* parent;
    std::vector<Node*> children;
    int id;
    int script_ref;
};