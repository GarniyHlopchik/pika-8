#include "lua_bindings.h"
#include "engine_context.h"
#include "../gfx/gfx.h"
#include "../gfx/text/text.h"

static int l_cls(lua_State* L){
    float r = luaL_optnumber(L,1,0.0f);
    float g = luaL_optnumber(L,2,0.0f);
    float b = luaL_optnumber(L,3,0.0f);
    glClearColor(r,g,b,1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    return 0;
}

static int l_load(lua_State* L){
    EngineContext* ctx = get_ctx(L);
    const char* path = luaL_checkstring(L, 1);
    std::cout << "Loading " << path << std::endl;
    const unsigned int id = ctx->gfx->load_texture(path);
    
    LoadedImages img;
    img.id = id;
    img.path = path;
    GFX::add_new_image(img);

    std::cout << "Loaded id " << id << std::endl;
    lua_pushnumber(L,id);
    return 1;
}
struct SpriteCut{
    float x1, x2, y1, y2;
};

static UVCoords calculate_uv(SpriteCut sprite, unsigned int texture) {
    UVCoords uv;
    if (sprite.x2 > sprite.x1 && sprite.y2 > sprite.y1) {
        std::vector<int> dims = GFX::get_image_dimensions(texture);
        float tex_w = (float)dims[0];
        float tex_h = (float)dims[1];

        // Protect against division by zero
        if (tex_w > 0.0f && tex_h > 0.0f) {
            // UVs are normalized (0.0 to 1.0). Divide the pixel coordinate by the total dimension.
            uv.u1 = sprite.x1 / tex_w;
            uv.v1 = sprite.y1 / tex_h;
            uv.u2 = sprite.x2 / tex_w;
            uv.v2 = sprite.y2 / tex_h;
        } else {
            // Fallback if the texture fails to load properly
            uv.u1 = 0.0f; uv.v1 = 0.0f; uv.u2 = 1.0f; uv.v2 = 1.0f;
        }
    } else {
        // Default UV coordinates (draws the entire texture)
        uv.u1 = 0.0f; uv.v1 = 0.0f; 
        uv.u2 = 1.0f; uv.v2 = 1.0f;
    }
    return uv;
}

static std::vector<float> get_table_floats(lua_State* L, int index, int count, float fallback = 1.0f) {
    
    // return a fallback value if NOT the table 
    if (!lua_istable(L, index)) {
        return std::vector<float>(count, fallback);
    }
    
    std::vector<float> values;
    values.reserve(count); // reserve space on RAM

    for (int i = 1; i <= count; i++) {
        // Pushes the value at table[i] to the top of the stack (-1)
        lua_rawgeti(L, index, i);
        // If the table is empty {}, optnumber will use the fallback
        values.push_back((float)luaL_optnumber(L, -1, fallback));
        // Pop the value immediately to keep the stack clean
        lua_pop(L, 1);
    }
    
    return values;
}

static Color get_color(lua_State* L, int index, float default_color = 255.0f) {
    std::vector<float> color_values = get_table_floats(L, index, 4, default_color);
    Color color = Color{
        color_values[0],
        color_values[1],
        color_values[2],
        color_values[3]
    };
    color.normalize(); // make colors between 0 and 1
    return color;
}

static SpriteCut get_sprite_cut(lua_State* L, int index) {
    std::vector<float> cut_values = get_table_floats(L, index, 4, 0.0f);
    SpriteCut cut = SpriteCut{
        cut_values[0],
        cut_values[1],
        cut_values[2],
        cut_values[3]
    };
    return cut;
}

/*
* Lua binding for drawing a sprite
* Arguments:
* 1. texture (number) - the texture ID to draw
* 2. x (number) - destination x position on screen
* 3. y (number) - destination y position on screen
* 4. width (number, optional) - destination width on screen (default: 8)
* 5. height (number, optional) - destination height on screen (default: 8)
* 6. sprite_x1 (number, optional) - starting X pixel on the sprite sheet (left)
* 7. sprite_x2 (number, optional) - ending X pixel on the sprite sheet (right)
* 8. sprite_y1 (number, optional) - starting Y pixel on the sprite sheet (top)
* 9. sprite_y2 (number, optional) - ending Y pixel on the sprite sheet (bottom)
*/
static int l_spr(lua_State* L) {
    EngineContext* ctx = get_ctx(L);
    // 1. Get required arguments (texture, x, y)
    const unsigned int texture = luaL_checknumber(L, 1);
    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    // 2. Get optional arguments (width, height) - defaulting to 8.0f if not provided
    float width = luaL_optnumber(L, 4, 8.0f);
    float height = luaL_optnumber(L, 5, 8.0f);

    // 3. (Optional) You can also expose the UV coordinates to Lua for sprite animations
    SpriteCut sprite = get_sprite_cut(L, 6); // expects a table {x1, x2, y1, y2}    

    Color color = get_color(L, 7); // expects a table {r, g, b, a}
    color.print("spr_color");

    UVCoords uv = calculate_uv(sprite, texture);

    // std::cout << u1 << v1 << u2 << v2 << std::endl;

    // Pass everything to your updated C++ draw function
    ctx->gfx->draw(texture, x, y, width, height, uv, color);
    
    return 0;
}

static int l_getscr(lua_State* L){
    EngineContext* ctx = get_ctx(L);
    auto [width,height] = ctx->gfx->get_screen_size();
    lua_pushnumber(L,width);
    lua_pushnumber(L,height);
    return 2;
}

static void draw_text(const char* text, int x, int y, 
    std::string font_name, float scale, Color color, float space_multiplier, Text* text_obj){
    try {
        text_obj->draw_text(text, x, y, font_name, scale, color, space_multiplier);
    } 
    catch (const std::exception& e) {
        if (std::string(e.what()).find("Font not found") != std::string::npos) {
            font_name = "default";
            std::cerr << "Warning: " << e.what() << " Falling back to default font." << std::endl;
            try {
                text_obj->draw_text(text, x, y, font_name, scale, color, space_multiplier);
            } catch (const std::exception& e) {
                std::cerr << "Error: Default font also not found. Cannot draw text." << std::endl;
            }
        } else {
            std::cerr << "Error in l_text: " << e.what() << std::endl;
        }
    }
}

/*
* Lua binding for drawing text
* Arguments:
* 1. text (string) - the text to draw
* 2. x (number) - x position
* 3. y (number) - y position
* 4. font_name (string) - name of the font to use
* 5. scale (number, optional) - scale of the text (default: 1.0)
* 6. color (table, optional) - color of the text as a table {r=..., g=..., b=..., a=...} (default: white)
* 7. space_multiplier (number, optional) - multiplier for space character width (default: 0.4)
*/
static int l_text(lua_State* L){
    EngineContext* ctx = get_ctx(L);
    const char* text = luaL_checkstring(L,1);
    float x = luaL_checknumber(L,2);
    float y = luaL_checknumber(L,3);
    float scale = luaL_optnumber(L, 4, 1.0f);
    std::string font_name = luaL_checkstring(L, 5);
    
    Color color = get_color(L, 6);

    float space_multiplier = luaL_optnumber(L, 7, 0.4f);
    
    draw_text(text, x, y, font_name, scale, color, space_multiplier, ctx->text);
    return 0;
}

static int l_close(lua_State* L){
    glfwSetWindowShouldClose(GFX::get_window(), GLFW_TRUE);
    return 0;
}

void bind_gfx(lua_State* L)
{
    static const luaL_Reg gfx_lib[] = {
        {"cls", l_cls},
        {"load", l_load},
        {"spr", l_spr},
        {"getscr", l_getscr},
        {"text", l_text},
        {"close", l_close},
        {NULL, NULL}
    };

    luaL_newlib(L, gfx_lib);
    lua_setglobal(L, "GFX");
}