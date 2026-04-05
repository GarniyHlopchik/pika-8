#include "config.h"
#include "../lua_bindings.h"
#include "../uv_utils.h"
#include "gfx/sprite/sprite.h"
#include "scene_tree/node2d.h"
#include "scene_tree/node.h"
void sol_bind_sprite(lua_State* L){
    
    sol::state_view lua(L);
    lua.new_usertype<Sprite>("Sprite",
        sol::base_classes, sol::bases<Node2D, Node>(),
        "pos", [](Sprite& self, float x, float y){
            self.update_position(x,y);
        },
        "size", [](Sprite& self, int width, int height){
            self.update_size(width, height);
        },
        "scaleXY", [](Sprite& self, float scale_x, float scale_y){
            float new_width = self.get_width() * scale_x;
            float new_height = self.get_height() * scale_y;
            self.update_size(new_width, new_height);
        },
        "scale", [](Sprite& self, float scale){
            float new_width = self.get_width() * scale;
            float new_height = self.get_height() * scale;
            self.update_size(new_width, new_height);
        },
        "pivot", [](Sprite& self, float x, float y){
            self.update_pivot(x, y);
        },
        "color", [](Sprite& self, sol::table color_table){
            Color color;
            color.r = color_table[1];
            color.g = color_table[2];
            color.b = color_table[3];
            color.a = color_table[4];
            self.update_color(color);
        },
        "uv", [](sol::this_state ts, Sprite& self, sol::table new_uv){
            UVCoords uv;
            uv.u1 = new_uv[1]; 
            uv.v1 = new_uv[2];
            uv.u2 = new_uv[3];
            uv.v2 = new_uv[4];
            self.update_uv(normalize_sprite_uv(uv, self.get_texture()));
        },
        "texture", [](Sprite& self, unsigned int texture){
            self.update_texture(texture);
        },
        "visible",[](Sprite& self, bool visible){
            self.update_visibility(visible);
        },
        "mirror", [](Sprite& self, bool horizontal, bool vertical){
            self.mirror(horizontal, vertical);
        },
        "rotation", [](Sprite& self, int degrees){
            self.update_rotation(degrees);
        },
        "update", [](sol::this_state ts, Sprite& self, int x,int y,int width,int height,sol::table uv_table,sol::table color_table){
            UVCoords uv;
            uv.u1 = uv_table[1];
            uv.v1 = uv_table[2];
            uv.u2 = uv_table[3];
            uv.v2 = uv_table[4];
            Color color;
            color.r = color_table[1];
            color.g = color_table[2];
            color.b = color_table[3];
            color.a = color_table[4];
            self.update(x,y,width,height,normalize_sprite_uv(uv, self.get_texture()),color);
        },
        "draw",[](Sprite& self,sol::this_state ts){
            lua_State* L = ts;
            EngineContext* ctx = get_ctx(L);
            self.draw(ctx->gfx);
        },
        "destroy",[](Sprite& self){
            self.~Sprite();
        }
    );


}