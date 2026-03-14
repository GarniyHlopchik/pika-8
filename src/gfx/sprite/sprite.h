#pragma once
#include "../gfx.h"

struct Sprite {
private:
    float _x, _y, _width, _height;
    unsigned int _texture;
    bool _dirty = true; // change has been made that requires redraw
    UVCoords _uv;
    Color _color;

public:
    Sprite(unsigned int texture, float x, float y, float width, float height, UVCoords uv, Color color);

    void update_position(float x, float y);
    void update_size(float width, float height);
    void update_color(Color color);
    void update_uv(UVCoords uv);
    void update_texture(unsigned int texture);

    void update(float x, float y, float width, float height, UVCoords uv, Color color);
    void draw(GFX* gfx);
};
