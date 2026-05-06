#pragma once

#include "user_input/user_input.h"
#include "gfx/sprite/sprite.h"
#include <sol/sol.hpp>

namespace button{
    class Button : public Sprite {
    public:
        Button(int x, int y, int width, int height, unsigned int id, LuaSystem* p_L, SceneTree* p_tree, unsigned int texture = 0, UVCoords uv = {0, 0, 1, 1}, Color color = Color()) 
            : Sprite(id, p_L, p_tree, texture, static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height), uv, color),
              _x(x), _y(y), _width(width), _height(height), _id(id) {}; 
    
        ~Button() = default;
    
        bool isPressed(InputState state);
        bool isDown(InputState state);
    
        bool containsCursor(float x, float y) const;
    
        // Set a Lua function to be called when button is pressed
        void setOnPressCallback(sol::function callback);
        void setOnHoldCallback(sol::function callback);

		void draw(GFX* gfx);
    
    private:
        int _x, _y, _width, _height;
        unsigned int _id;
    
        sol::function onPressCallback;
        sol::function onHoldCallback;
    };
}
