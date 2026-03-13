#pragma once
#include "config.h"
class Node;
class LuaSystem;
class SceneTree{
    public:
    SceneTree(LuaSystem* p_lua);
    pugi::xml_document find_in_scenes(const std::string& name);
    void change_scene(const std::string& name); // does find in scenes, queue_frees all old nodes,adds new ones
    void remove_root();
    void set_root(std::unique_ptr<Node> p_root);
    void update(float dt);
    private:
    std::unique_ptr<Node> root;
    LuaSystem* lua;
};