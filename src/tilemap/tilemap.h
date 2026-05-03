#pragma once
#include <vector>

namespace tilemap {

    struct Tile {
        int id = 0;      
        bool solid = false;  
    };

    class Tilemap {
    public:
        Tilemap();
        ~Tilemap();

        void create(int width, int height, int tile_size = 16);

        void set_tile(int x, int y, int tile_id, bool solid = false);
        int get_tile(int x, int y) const;
        bool is_solid(int x, int y) const;

        int get_width() const { return m_width; }
        int get_height() const { return m_height; }
        int get_tile_size() const { return m_tile_size; }

        void clear();

    private:
        int m_width = 0;
        int m_height = 0;
        int m_tile_size = 16;
        std::vector<Tile> m_tiles;
    };
} 