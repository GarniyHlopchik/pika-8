#include "sprite.h"

Sprite::Sprite(unsigned int texture, float x, float y, float width, float height, UVCoords uv, Color color)
    : _texture(texture), _x(x), _y(y), _width(width), _height(height), _uv(uv), _color(color) {}

void Sprite::update_position(float x, float y) {
    if (x != _x || y != _y) {
        _x = x;
        _y = y;
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
    if (color != _color) {
        _color = color;
        _dirty = true;
    }
}

void Sprite::update_uv(UVCoords uv) {
    if (uv.u1 != _uv.u1 || uv.v1 != _uv.v1 || uv.u2 != _uv.u2 || uv.v2 != _uv.v2) {
        _uv = uv;
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

void Sprite::update_rotation(float degrees) {
    if (degrees != _rotation) {
        _rotation = degrees;
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
    // if (_dirty) { // Optional optimization: only redraw if something changed /// removed for causing flickering due to redrawing the window every frame
        gfx->draw(_texture, _x, _y, _width, _height, _uv, _color);
        _dirty = false; // Reset dirty flag after drawing
    // }
    }
}