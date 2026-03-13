#pragma once
#include "config.h"


class Sprite{
    public:
    Sprite(int x1, int y1, int x2, int y2);
    ~Sprite() = default;
    void set_borders(int x1, int y1, int x2, int y2);
    std::tuple<int,int,int,int> get_borders();

    private:
    int x1, x2, y1, y2;
};

class SpriteSheet{
    public:
    SpriteSheet(std::array<uint32_t,16>& palette);
    ~SpriteSheet() = default;
    std::vector<uint8_t> get_rgba_array(); //sheet is 128x32=4096px, each px is 4 elements -> 16 384 elements
    std::vector<Sprite>& get_sprites();
    void set_pixel(int x, int y, int color_ind);
    void set_palette(const std::array<uint32_t,16>& palette); //for setting when palette changes in editor
    void add_sprite(Sprite& sprite);
    void remove_sprite(size_t index);
    
    private:
    std::array<uint8_t, 4096> pixels; //color index 0-16, here it's element per pixel so only( :) ) 4096
    std::array<uint32_t, 16> palette;
    std::vector<Sprite> sprites;
};