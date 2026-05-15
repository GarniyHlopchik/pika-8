#pragma once
#include <string>
#include <functional>
#include <memory>
#include <fstream>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>

namespace tilemap {

    struct TileChunk {
        int chunk_x = 0;          
        int chunk_y = 0;         
        int width = 0;            
        int height = 0;           
        std::vector<int> tiles;   
        bool is_last = false;     
        bool is_error = false;    
        std::string error_msg;   

        int get_tile(int x, int y) const {
            if (x < 0 || x >= width || y < 0 || y >= height) return 0;
            return tiles[y * width + x];
        }
    };

    class TilemapStream {
    public:
        TilemapStream();
        ~TilemapStream();

        void open(const std::string& path, int chunk_size = 16);

        void close();

        bool has_next() const;

        TileChunk next();

        void on_chunk(std::function<void(const TileChunk&)> callback);

        float progress() const;

        int get_width() const { return m_width; }
        int get_height() const { return m_height; }

    private:
        void worker_thread();
        void parse_header();
        TileChunk read_next_chunk();
        int char_to_tile(char c) const;

        std::thread m_worker;
        std::atomic<bool> m_running;
        std::atomic<bool> m_has_more;

        std::queue<TileChunk> m_chunks;
        mutable std::mutex m_mutex;
        std::condition_variable m_cv;

        std::string m_path;
        std::ifstream m_file;
        int m_width = 0;
        int m_height = 0;
        int m_chunk_size = 16;
        int m_total_chunks = 0;
        int m_loaded_chunks = 0;

        std::function<void(const TileChunk&)> m_callback;
    };

    class StreamingTilemapBuilder {
    public:
        using TileHandler = std::function<void(int x, int y, int tile_id)>;

        StreamingTilemapBuilder(int tile_size = 16, int chunk_size = 16);
        ~StreamingTilemapBuilder();

        void build(TilemapStream& stream, TileHandler handler);
        float progress() const { return m_progress; }

    private:
        int m_tile_size;
        int m_chunk_size;
        int m_total_tiles;
        int m_processed_tiles;
        float m_progress;
    };
} 