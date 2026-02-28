#include "scene_tree.h"

SceneTree::SceneTree(LuaSystem* p_lua){
    lua = p_lua;
}
pugi::xml_document SceneTree::find_in_scenes(const std::string& name){}
void SceneTree::change_scene(const std::string& name){}
void SceneTree::remove_root(){
    root.reset();
}
void SceneTree::set_root(std::unique_ptr<Node> p_root){
    root = std::move(p_root);
}
void SceneTree::update(float dt){
    lua->call_script_update(dt);
}