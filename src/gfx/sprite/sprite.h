#pragma once
#include "globals.h"
#include "gfx/gfx.h"
#include "scene_tree/node2d.h"
#include "debug/debug.h"

/// TODO: More sprite functions
///         animate(bool)       - is the sprite animated, 
///         repeat(bool)        - repeat the animation continuously
///         frame(int)          - skips to the frame


class Sprite : public Node2D {
public:
    Sprite(int p_id, LuaSystem* p_L, SceneTree* p_tree, unsigned int texture, float x, float y, float width, float height, UVCoords uv, Color color);

    void update_position(float x, float y);
    void update_size(float width, float height);
    void update_color(Color color);
    void update_uv(UVCoords uv);
    void update_texture(unsigned int texture);
    void update_visibility(bool visible);
    
    void update_pivot(float x, float y);
    void update_rotation(int degrees);
    void mirror(bool horizontal, bool vertical);


    void update(float x, float y, float width, float height, UVCoords uv, Color color);
    void draw(GFX* gfx);

    void _update(float delta) override; //override of Node's _update, calls draw() 

    float get_width() const { return _width; }
    float get_height() const { return _height; }
    float get_x() const { return _x; }
    float get_y() const { return _y; }
    float get_rotation() const { return _rotation; }
    unsigned int get_texture() const { return _texture; }
    bool is_visible() const { return _visible; }
    UVCoords get_uv() const { return _uv; }
    Color get_color() const { return _color; }
    PivotPoint get_pivot() const { return _pivot; }

    void push_to_lua(lua_State* L) override {
        sol::stack::push(L, std::ref(*this));
    }
private:
    unsigned int _texture;
    float _x, _y;
    float _width, _height;

    int _rotation = 0;

    bool _dirty = true; // change has been made that requires redraw
    bool _visible = true; 
    
    PivotPoint _pivot = { 0.0f, 0.0f };
    UVCoords _uv;
    Color _color;
};
