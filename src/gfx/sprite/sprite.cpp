#include "sprite.h"
#include "lua_bindings/engine_context.h"
Sprite::Sprite(int p_id, LuaSystem* p_L, SceneTree* p_tree, unsigned int texture, float x, float y, float width, float height, UVCoords uv, Color color)
    : Node2D(p_id, p_L, p_tree, x, y), _texture(texture), _x(x), _y(y), _width(width), _height(height), _uv(uv), _color(color) {}

void Sprite::update_position(float x, float y) {
    if (x != _x || y != _y) {
        _x = x;
        _y = y;
        position.x = x;
        position.y = y;
        _dirty = true;
    }
}

void Sprite::update_size(float width, float height) {
    if (width != _width || height != _height) {
        _width = width;
        _height = height;
        _dirty = true;
    }
}

void Sprite::update_color(Color color) {
    static const Color white = {1.0f, 1.0f, 1.0f, 1.0f};
    static const Color black = {0.0f, 0.0f, 0.0f, 1.0f};

    if (color != _color) {
        _color = color;
        
        if (color > white) color = white;
        if (color < black) color = black;

        _dirty = true;
    }
}

void Sprite::update_uv(UVCoords uv) {
    if (uv.u1 != _uv.u1 || uv.v1 != _uv.v1 || uv.u2 != _uv.u2 || uv.v2 != _uv.v2) {
        _uv = uv;

        // Pivot dynamic update when uv is updated
        // float new_width = (_uv.u2 - _uv.u1) * 0.5;
        // float new_height = (_uv.v2 - _uv.v1) * 0.5;
        
        // float normalized_pivot_x = new_width / (_width * 0.5);
        // float normalized_pivot_y = new_height / (_height * 0.5);

        // std::cout << "pivot: {" << normalized_pivot_x << ", " << normalized_pivot_y << "}";

        // update_pivot(normalized_pivot_x, normalized_pivot_y);
        _dirty = true;
    }
}

void Sprite::update_texture(unsigned int texture) {
    if (texture != _texture) {
        _texture = texture;
        _dirty = true;
    }
}

void Sprite::update_visibility(bool visible) {
    if (visible != _visible) {
        _visible = visible;
        _dirty = true;
    }
}



void Sprite::update_rotation(int degrees) {
    int normalized = (degrees % 360 + 360) % 360;

    if (normalized != _rotation) {
        _rotation = normalized;
        _dirty = true;
    }
}

void Sprite::update_pivot(float x, float y){
    if (x != _pivot.x || y != _pivot.y) { 
        _pivot.x = x;
        _pivot.y = y;
        _dirty = true;
    }
}

void Sprite::mirror(bool horizontal, bool vertical) {
    if (horizontal) {
        std::swap(_uv.u1, _uv.u2);
    }
    if (vertical) {
        std::swap(_uv.v1, _uv.v2);
    }
    _dirty = true;
}



void Sprite::update(float x, float y, float width, float height, UVCoords uv, Color color) {
    update_position(x, y);
    update_size(width, height);
    update_uv(uv);
    update_color(color);
}

void Sprite::draw(GFX* gfx) {
    if(_visible){
        // std::cout << _texture << "'s rotation: " << _rotation << std::endl;
        gfx->draw(*this);
        // std::cout << this->get_x() << " " << this->get_y() << '\n';
        _dirty = false; // Reset dirty flag after drawing
    }
}
void Sprite::_update(float delta){
    Node::_update(delta);
    draw(get_ctx(lua->get_state())->gfx);
}