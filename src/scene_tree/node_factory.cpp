#include "node_factory.h"

std::unique_ptr<Node> NodeFactory::create_node(LuaSystem* lua,SceneTree* tree, std::string& script_name){

        int id = NodeFactory::get_id();
        std::unique_ptr<Node> node = std::make_unique<Node>(id,lua,tree);
        
        if(script_name!=""){
            int script_ref = lua->load_script_table(script_name);
            node->_init(script_ref);
        }

        return node;
    
}
std::unique_ptr<YSort> NodeFactory::create_ysort(LuaSystem* lua,SceneTree* tree, std::string& script_name, float x, float y){

        int id = NodeFactory::get_id();
        std::unique_ptr<YSort> node = std::make_unique<YSort>(id,lua,tree,x,y);
        
        if(script_name!=""){
            int script_ref = lua->load_script_table(script_name);
            node->_init(script_ref);
        }

        return node;
    
}
std::unique_ptr<Sprite> NodeFactory::create_sprite(LuaSystem* lua,SceneTree* tree,std::string& script_name, 
    unsigned int texture, float x, float y, float width, float height, UVCoords uv, Color color){
        int id = NodeFactory::get_id();
        std::unique_ptr<Sprite> node = std::make_unique<Sprite>(id,lua,tree,texture,x,y,width,height,uv,color);
        
        if(script_name!=""){
            int script_ref = lua->load_script_table(script_name);
            node->_init(script_ref);
        }

        return node;
    }
std::unique_ptr<Node2D> NodeFactory::create_node2d(LuaSystem* lua,SceneTree* tree,std::string& script_name,float x, float y){
    int id = NodeFactory::get_id();
    std::unique_ptr<Node2D> node = std::make_unique<Node2D>(id,lua,tree,x,y);
        
    if(script_name!=""){
        int script_ref = lua->load_script_table(script_name);
        node->_init(script_ref);
    }

    return node;
}
int NodeFactory::get_id(){
    static int id = 1;
    return ++id;
}