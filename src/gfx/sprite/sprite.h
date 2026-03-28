#pragma once
#include "../gfx.h"
#include "scene_tree/node2d.h"

class Sprite : public Node2D{
public:
    Sprite(int p_id, LuaSystem* p_L, SceneTree* p_tree, unsigned int texture, float x, float y, float width, float height, UVCoords uv, Color color);

    void update_position(float x, float y);
    void update_size(float width, float height);
    void update_color(Color color);
    void update_uv(UVCoords uv);
    void update_texture(unsigned int texture);
    void update_visibility(bool visible);
    void update_rotation(float degrees);
    void mirror(bool horizontal, bool vertical);

    void update(float x, float y, float width, float height, UVCoords uv, Color color);
    void draw(GFX* gfx);


    float get_width() const { return _width; }
    float get_height() const { return _height; }
    float get_x() const { return position.x; }
    float get_y() const { return position.y; }
    unsigned int get_texture() const { return _texture; }
    bool is_visible() const { return _visible; }
    UVCoords get_uv() const { return _uv; }
    Color get_color() const { return _color; }

    void push_to_lua(lua_State* L) override {
        // This tells sol2: "I am specifically a Sprite"
        sol::stack::push(L, std::ref(*this));
    }

private:
    float _width, _height, _rotation = 0.0f;
    unsigned int _texture;
    bool _dirty = true, _visible = true; // change has been made that requires redraw
    UVCoords _uv;
    Color _color;
};
