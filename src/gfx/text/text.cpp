#define STB_TRUETYPE_IMPLEMENTATION
#include "text.h"
#include "../sprite/sprite.h"

#include <vector>

Text::Text(GFX& p_gfx) 
    : gfx(p_gfx)  // This binds the reference. You're done!
{
    //deleted the gfx assignment cause it fucked compilation - denos
}
Text::~Text() {
    // Destructor can be used to clean up font textures if needed
    for (const auto& [name, font] : font_cache) {
        glDeleteTextures(1, &font.texture_id);
    }
}

bool Text::is_font_loaded(const std::string& font_name){
    return (font_cache.find(font_name) != font_cache.end());
}

void Text::load_font(const std::string& font_name) {
    FontData font_config = gfx.get_font_data(font_name); 
    
    // load the raw .ttf
    Resource res = FileSystem::get_resource(font_config.path);
    if (!res.is_valid()) return;

    // prepare a bitmap size for the alpha channel
    const int atlas_w = 2048, atlas_h = 2048;
    unsigned char* temp_bitmap = new unsigned char[atlas_w * atlas_h];

    const float BAKED_PIXEL_HEIGHT = 256.0f; // 8 times smaller than atlas... breacks otherwise.... but it is funny

    // NOTE: '32' is the start char (Space), '96' is the count of chars to bake
    stbtt_BakeFontBitmap(res.data.get(), 0, BAKED_PIXEL_HEIGHT, temp_bitmap, atlas_w, atlas_h, 32, 96, font_config.cdata);

    // upload to GPU
    unsigned int tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas_w, atlas_h, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    font_cache[font_name] = { tex_id, (unsigned int)atlas_w, (unsigned int)atlas_h, font_config };
}


void Text::draw_text(const std::string& text, float x, float y, const std::string& font_name, float scale, Color color) {
    if (!is_font_loaded(font_name)) load_font(font_name);
    if (!is_font_loaded(font_name)) return;
    
    const LoadedFont& font = font_cache.at(font_name);

    float local_x = 0.0f;
    float local_y = 0.0f;

    for (char c : text) {
        if (c < 32 || c > 127) continue;

        stbtt_aligned_quad q;
        
        stbtt_GetBakedQuad(font.data.cdata, font.width, font.height, c - 32, &local_x, &local_y, &q, 1);

        float w = (q.x1 - q.x0) * scale;
        float h = (q.y1 - q.y0) * scale;
        
        // shift the character to be centered on its position
        float final_x = x + (q.x0 * scale) + (w / 2.0f);
        float final_y = y + (q.y0 * scale) + (h / 2.0f);

        UVCoords uv = { q.s0, q.t0, q.s1, q.t1 };

        Sprite char_sprite(font.texture_id, nullptr, nullptr, font.texture_id, 
                           final_x, final_y, w, h, uv, color);

        gfx.draw(char_sprite);
    }
}