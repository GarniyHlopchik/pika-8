#define _USE_MATH_DEFINES
#include "../engine_context.h"
#include "../lua_bindings.h"
#include "debug/debug.h"
#include "../table_getter/table_gatter.h"
#include <vector>
#include <cmath>
#include <utility>


typedef std::pair<float, float> Vec2;

static int l_draw_line(lua_State* L){
    std::vector<float> pos1 = get_table_floats(L,1,2);
    std::vector<float> pos2 = get_table_floats(L,2,2);

    Color color = get_color(L, 3);

    Debug::draw_line(pos1[0],pos1[1],pos2[0],pos2[1],color);
    return 0;
}

static int l_draw_rect(lua_State* L){
    std::vector<float> pos = get_table_floats(L,1,2);
    std::vector<float> dimensions = get_table_floats(L,2,2);

    Color color = get_color(L, 3);

    Debug::draw_rect(pos[0],pos[1],dimensions[0],dimensions[1],color);
    return 0;
}

static int l_draw_circle(lua_State* L){
    std::vector<float> pos = get_table_floats(L,1,2);
    float radius = luaL_checknumber(L,2);

    Color color = get_color(L, 3);

    Debug::draw_circle(pos[0],pos[1],radius,color);
    return 0;
}

static int l_draw_custom_shape(lua_State* L){
    int amount = luaL_checknumber(L,1);

    std::vector<std::pair<float, float>> positions;
    for(int i = 0; i < amount; i++){
        std::vector<float> pos = get_table_floats(L,i+2,2);
        auto pair = std::make_pair(pos[0],pos[1]);
        positions.push_back(pair);
    }

    Color color = get_color(L, amount+2);

    Debug::draw_polygon(positions,color);
    return 0;
}

std::vector<Vec2> generatePolygon(int number_of_peaks, float radius, Vec2 center) {
    if (number_of_peaks < 3) return {};

    std::vector<Vec2> vertices;
    vertices.reserve(number_of_peaks);


    for (int i = 0; i < number_of_peaks; ++i) {
        float angle = i * (2.0f * M_PI / number_of_peaks);

        float x = center.first + radius * std::cos(angle);
        float y = center.second + radius * std::sin(angle);

        auto pair = std::make_pair(x, y);
        vertices.push_back(pair);
    }

    return vertices;
}

/// Lua: Debug::draw_polygon( amount, side_length, { x, y }, { r, g, b, a } )
static int l_draw_polygon(lua_State* L){
    // culculate all positions based on the amount of vertices and vertecy lengths    
    int side_amount = luaL_checknumber(L,1);
    if (side_amount < 3) {
        return 0;
    }

    int side_length = luaL_checknumber(L,2);    
    if (side_length <= 0) {
        return 0;
    }

    Vec2 center_position = get_vec2(L,3);

    const std::vector<Vec2> vertecies = generatePolygon(side_amount, side_length, center_position);

    // colors
    // std::vector<float> parse_color = get_table_floats(L,amount+2,4, 1.0);
    // color.r = parse_color[0];
    // color.g = parse_color[1];
    // color.b = parse_color[2];
    // color.a = parse_color[3];

    Color color = get_color(L, 4);
    
    Debug::draw_polygon(vertecies, color);
    return 0;
}

void bind_debug(lua_State* L){
    static const luaL_Reg debug_lib[] = {
        {"draw_line", l_draw_line},
        {"draw_rect", l_draw_rect},
        {"draw_circle", l_draw_circle},
        {"draw_polygon", l_draw_polygon},
        {"draw_custom_shape", l_draw_custom_shape},
        {NULL, NULL}
    };

    luaL_newlib(L, debug_lib);
    lua_setglobal(L, "Debug");
}