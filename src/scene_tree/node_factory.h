#include "config.h"
#include "scene_tree/node.h"
#include "scene_tree/node2d.h"
#include "gfx/sprite/sprite.h"
class LuaSystem;
class SceneTree;
class Node;
class Sprite;
class NodeFactory{
    public:
    static std::unique_ptr<Node2D> create_node2d(LuaSystem* lua,SceneTree* tree,std::string& script_name,float x, float y);
    static std::unique_ptr<Node> create_node(LuaSystem* lua,SceneTree* tree,std::string& script_name);
    static std::unique_ptr<Sprite> create_sprite(LuaSystem* lua,SceneTree* tree,std::string& script_name, 
    unsigned int texture, float x, float y, float width, float height, UVCoords uv, Color color);
    static int get_id();
};