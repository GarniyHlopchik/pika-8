#include "../engine_context.h"
#include "../lua_bindings.h"
#include "debug/debug.h"
#include "../table_getter/table_gatter.h"
#include <vector>
static int l_draw_line(lua_State* L){
    std::vector<float> pos1 = get_table_floats(L,1,2);
    std::vector<float> pos2 = get_table_floats(L,2,2);
    std::vector<float> parse_color = get_table_floats(L,3,4, 1.0);
    Color color;
    color.r = parse_color[0];
    color.g = parse_color[1];
    color.b = parse_color[2];
    color.a = parse_color[3];
    Debug::draw_line(pos1[0],pos1[1],pos2[0],pos2[1],color);
    return 0;
}
static int l_draw_rect(lua_State* L){
    std::vector<float> pos = get_table_floats(L,1,2);
    std::vector<float> dimensions = get_table_floats(L,2,2);
    std::vector<float> parse_color = get_table_floats(L,3,4, 1.0);
    Color color;
    color.r = parse_color[0];
    color.g = parse_color[1];
    color.b = parse_color[2];
    color.a = parse_color[3];
    Debug::draw_rect(pos[0],pos[1],dimensions[0],dimensions[1],color);
    return 0;
}
static int l_draw_circle(lua_State* L){
    std::vector<float> pos = get_table_floats(L,1,2);
    float radius = luaL_checknumber(L,2);
    std::vector<float> parse_color = get_table_floats(L,3,4, 1.0);
    Color color;
    color.r = parse_color[0];
    color.g = parse_color[1];
    color.b = parse_color[2];
    color.a = parse_color[3];
    Debug::draw_circle(pos[0],pos[1],radius,color);
    return 0;
}
static int l_draw_polygon(lua_State* L){
    int amount = luaL_checknumber(L,1);
    std::vector<std::pair<float, float>> positions;
    for(int i = 0; i < amount; i++){
        std::vector<float> pos = get_table_floats(L,i+2,2);
        auto pair = std::make_pair(pos[0],pos[1]);
        positions.push_back(pair);
    }
    std::vector<float> parse_color = get_table_floats(L,amount+2,4, 1.0);
    Color color;
    color.r = parse_color[0];
    color.g = parse_color[1];
    color.b = parse_color[2];
    color.a = parse_color[3];
    Debug::draw_polygon(positions,color);
    return 0;
}
void bind_debug(lua_State* L){
    static const luaL_Reg debug_lib[] = {
        {"draw_line", l_draw_line},
        {"draw_rect", l_draw_rect},
        {"draw_circle", l_draw_circle},
        {"draw_polygon", l_draw_polygon},
        {NULL, NULL}
    };

    luaL_newlib(L, debug_lib);
    lua_setglobal(L, "Debug");
}