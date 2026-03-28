#include "node2d.h"

Vector2 Node2D::get_global_position(){
    if (Node2D* node = dynamic_cast<Node2D*>(parent)) {
        Vector2 pos;
        pos.x = position.x+node->position.x;
        pos.y = position.y+node->position.y;
        return pos;
    }
    else{
        return position;
    }
}