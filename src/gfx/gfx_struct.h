#pragma once
#include "sprite_mesh/mesh_struct.h"

struct LoadedFont {
    unsigned int texture_id;
    unsigned int width;
    unsigned int height;
    FontData data;
};

struct LoadedImages
{
    std::string path;
    unsigned int id;
};