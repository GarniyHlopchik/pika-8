#include "datatypes.h"

Sprite::Sprite(int _x1, int _y1, int _x2, int _y2){
    x1 = _x1;
    x2 = _x2;
    y1 = _y1;
    y2 = _y2;
}
void Sprite::set_borders(int _x1, int _y1, int _x2, int _y2){
    x1 = _x1;
    x2 = _x2;
    y1 = _y1;
    y2 = _y2;
}
std::tuple<int,int,int,int> Sprite::get_borders(){
    return std::make_tuple(x1, y1, x2, y2);
}
SpriteSheet::SpriteSheet(std::array<uint32_t,16>& palette){
    
}