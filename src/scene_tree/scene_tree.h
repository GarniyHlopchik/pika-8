#pragma once
#include "config.h"
class Node;
class SceneTree{
    public:
    
    ~SceneTree();
    pugi::xml_document find_in_scenes(const std::string& name);
    void change_scene(const std::string& name); // does find in scenes, queue_frees all old nodes,adds new ones

    void set_root(Node* root);
    void update(float dt);
    private:
    Node* root = nullptr;
    std::vector<Node*> deletion_queue;
    
};