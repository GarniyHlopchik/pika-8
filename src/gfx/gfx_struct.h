#pragma once
#include "../json_reader/get_config.h"
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


// async staff for texture loading/ threading communication between main thread and worker thread
struct TextureLoadRequest{
    int registry_ref;
    std::string path;
};

struct ImageData {
    unsigned char* pixel_data;
    int width;
    int height;
    int channels;
};

struct TextureLoadResult{
    int registry_ref;
    unsigned int texture_id;
    ImageData image;
    std::string path;
};