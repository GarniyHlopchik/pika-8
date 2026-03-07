#pragma once

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

struct Color
{
    int r, g, b, a;
};

struct UVCoords {
    float u1, v1, u2, v2;
};