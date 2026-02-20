#pragma once
#include "config.h"

class SceneTree{
    public:
    ~SceneTree();
    void set_root(Node* root);
    void update(float dt);
    private:
    Node* root = nullptr;
};