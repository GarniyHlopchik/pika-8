#pragma once

#include "../sprite_mesh/mesh_struct.h"
#include "../gfx/gfx.h"

inline UVCoords normalize_sprite_uv(UVCoords sprite, unsigned int texture) {
    UVCoords uv;
    if (sprite.u2 > sprite.u1 && sprite.v2 > sprite.v1) {
        if (sprite.u1 >= 0.0f && sprite.u2 <= 1.0f && sprite.v1 >= 0.0f && sprite.v2 <= 1.0f) {
            return sprite;
        }

        std::vector<int> dims = GFX::get_image_dimensions(texture);
        float tex_w = static_cast<float>(dims[0]);
        float tex_h = static_cast<float>(dims[1]);

        if (tex_w > 0.0f && tex_h > 0.0f) {
            uv.u1 = sprite.u1 / tex_w;
            uv.v1 = sprite.v1 / tex_h;
            uv.u2 = sprite.u2 / tex_w;
            uv.v2 = sprite.v2 / tex_h;
            return uv;
        }
    }

    uv.u1 = 0.0f;
    uv.v1 = 0.0f;
    uv.u2 = 1.0f;
    uv.v2 = 1.0f;
    return uv;
}
