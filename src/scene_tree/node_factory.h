#include "config.h"
#include "scene_tree/node.h"
class LuaSystem;
class SceneTree;
class Node;
class NodeFactory{
    public:
    static std::unique_ptr<Node> create(const std::string& type, LuaSystem* lua,SceneTree* tree,std::string& script_name);
    static int get_id();
};