#include "text.h"

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

void Text::load_font(const std::string& font_name){
    FontData font_data = gfx.get_font_data(font_name);    
    unsigned int tex_id = gfx.load_texture(font_data.path);
    
    if (!tex_id) return;

    unsigned int img_width = 0, img_height = 0;
    std::vector<int> dims = gfx.get_image_dimensions(font_data.path);
    img_width = dims[0];
    img_height = dims[1];

    font_cache[font_name] = {tex_id, img_width, img_height, font_data};
}

size_t Text::get_char_index(char c, const std::string& charset) const {
    size_t index = charset.find(c);
    
    // Fallback to uppercase if lowercase isn't found
    if (index == std::string::npos) {
        char upper_c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
        index = charset.find(upper_c);
    }
    
    return index;
}

UVCoords Text::calculate_uv_coords(size_t index, const LoadedFont& font) const {
    const int chars_per_row = font.width / font.data.char_width;
    const int total_rows = font.height / font.data.char_height;

    int col = index % chars_per_row;
    int row = index / chars_per_row;
    
    // Note: Are you sure these shouldn't be font.width / font.height?
    /// Should be exactly that. No clue how it worked....
    const float tex_width = font.width; 
    const float tex_height = font.height;

    const float margin_u = 0.1f / tex_width;
    const float margin_v = 0.1f / tex_height;
        
    return {
        (static_cast<float>(col) / chars_per_row) + margin_u,
        (static_cast<float>(row) / total_rows) + margin_v,
        (static_cast<float>(col + 1) / chars_per_row) - margin_u,
        (static_cast<float>(row + 1) / total_rows) - margin_v
    };
}

void Text::draw_text(const std::string& text, float x, float y, const std::string& font_name, float scale, Color color, float space_multiplier) {
    // load font to cash onse
    if (is_font_loaded(font_name) == false) {
        load_font(font_name);
    }
    
    if (is_font_loaded(font_name) == false) return;

    const LoadedFont& font = font_cache[font_name];

    float draw_width = font.data.char_width * scale; 
    float draw_height = font.data.char_height * scale; 

    float cursor_x = x; 

    // Drawing text by charecter
    for (char c : text) {
        if (c == ' ') {
            cursor_x += (draw_width * space_multiplier); 
            continue;
        }

        size_t index = get_char_index(c, font.data.charset);
        if (index == std::string::npos) continue;
        
        UVCoords uv = calculate_uv_coords(index, font);

        // Use the cached texture ID
        gfx.draw(font.texture_id, cursor_x, y, draw_width, draw_height, uv, color);

        cursor_x += draw_width; 
    }
}