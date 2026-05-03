#include "tilemap.h"
#include <iostream>

namespace tilemap {

    Tilemap::Tilemap() = default;
    Tilemap::~Tilemap() = default;

    void Tilemap::create(int width, int height, int tile_size) {
        m_width = width;
        m_height = height;
        m_tile_size = tile_size;
        m_tiles.resize(width * height);
    }

    void Tilemap::set_tile(int x, int y, int tile_id, bool solid) {
        if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
            return;  
        }
        int idx = y * m_width + x;
        m_tiles[idx].id = tile_id;
        m_tiles[idx].solid = solid;
    }

    int Tilemap::get_tile(int x, int y) const {
        if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
            return 0;
        }
        return m_tiles[y * m_width + x].id;
    }

    bool Tilemap::is_solid(int x, int y) const {
        if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
            return true; 
        }
        return m_tiles[y * m_width + x].solid;
    }

    void Tilemap::clear() {
        for (auto& tile : m_tiles) {
            tile.id = 0;
            tile.solid = false;
        }
    }

} 