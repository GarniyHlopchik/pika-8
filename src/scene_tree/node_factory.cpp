#include "node_factory.h"
class LuaSystem;
class SceneTree;
class Node;
std::unique_ptr<Node> NodeFactory::create(const std::string& type, LuaSystem* lua,SceneTree* tree, std::string& script_name){

    if(type=="node"){
        int id = NodeFactory::get_id();
        std::unique_ptr<Node> node = std::make_unique<Node>(id,lua,tree);
        
        if(script_name!=""){
            node->script_ref = lua->load_script_table(script_name);
        }

        return node;
    }
    else{
        std::cout << "No such node name found: " << type << std::endl;
    }
}
int NodeFactory::get_id(){
    static int id = 1;
    return ++id;
}