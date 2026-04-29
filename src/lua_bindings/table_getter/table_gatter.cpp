#include "table_gatter.h"

/// @brief Gets a vector of floats from a Lua table
/// @param L Lua state
/// @param index index of the table on the Lua stack
/// @param count number of floats to retrieve from the table
/// @param fallback fallback value if the table is not found or empty
/// @return 
std::vector<float> get_table_floats(lua_State* L, int index, int count, float fallback) {
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

std::vector<int> get_table_ints(lua_State* L, int index, int count, int fallback) {
    
    // return a fallback value if NOT the table 
    if (!lua_istable(L, index)) {
        return std::vector<int>(count, fallback);
    }
    
    std::vector<int> values;
    values.reserve(count); // reserve space on RAM

    for (int i = 1; i <= count; i++) {
        // Pushes the value at table[i] to the top of the stack (-1)
        lua_rawgeti(L, index, i);
        // If the table is empty {}, optnumber will use the fallback
        values.push_back((int)luaL_optinteger(L, -1, fallback));
        // Pop the value immediately to keep the stack clean
        lua_pop(L, 1);
    }
    
    return values;
}

Color get_color(lua_State* L, int index, float default_color) {
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

UVCoords get_sprite_cut(lua_State* L, int index) {
    std::vector<float> cut_values = get_table_floats(L, index, 4, 0.0f);
    UVCoords cut = UVCoords{
        cut_values[0],
        cut_values[1],
        cut_values[2],
        cut_values[3]
    };
    return cut;
}

std::pair<float, float> get_vec2(lua_State* L, int index) {
    std::vector<float> vec_values = get_table_floats(L, index, 2, 0.0f);
    return std::make_pair(vec_values[0], vec_values[1]);
}
