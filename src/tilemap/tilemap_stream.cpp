#include "tilemap_stream.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <chrono>
#include <thread>

namespace tilemap {

    TilemapStream::TilemapStream()
        : m_running(false)
        , m_has_more(false)
        , m_width(0)
        , m_height(0)
        , m_total_chunks(0)
        , m_loaded_chunks(0)
    {
    }

    TilemapStream::~TilemapStream() {
        close();
    }

    void TilemapStream::open(const std::string& path, int chunk_size) {
        if (m_running) close();

        m_path = path;
        m_chunk_size = chunk_size;
        m_running = true;
        m_has_more = true;
        m_loaded_chunks = 0;

        m_file.open(m_path);
        if (!m_file.is_open()) {
            TileChunk error_chunk;
            error_chunk.is_error = true;
            error_chunk.error_msg = "Cannot open file: " + m_path;
            error_chunk.is_last = true;

            std::lock_guard<std::mutex> lock(m_mutex);
            m_chunks.push(error_chunk);
            m_cv.notify_one();
            return;
        }

        parse_header();

        int chunks_x = (m_width + m_chunk_size - 1) / m_chunk_size;
        int chunks_y = (m_height + m_chunk_size - 1) / m_chunk_size;
        m_total_chunks = chunks_x * chunks_y;

        m_worker = std::thread(&TilemapStream::worker_thread, this);
    }

    void TilemapStream::close() {
        if (m_running) {
            m_running = false;
            m_cv.notify_all();
            if (m_worker.joinable()) {
                m_worker.join();
            }
        }
        if (m_file.is_open()) {
            m_file.close();
        }
    }

    bool TilemapStream::has_next() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return !m_chunks.empty() || m_has_more;
    }

    TileChunk TilemapStream::next() {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_cv.wait(lock, [this] {
            return !m_chunks.empty() || !m_running;
            });

        if (m_chunks.empty()) {
            TileChunk empty;
            empty.is_last = true;
            return empty;
        }

        TileChunk chunk = std::move(m_chunks.front());
        m_chunks.pop();
        return chunk;
    }

    void TilemapStream::on_chunk(std::function<void(const TileChunk&)> callback) {
        m_callback = callback;

        if (!m_running) {
            m_running = true;
            m_worker = std::thread(&TilemapStream::worker_thread, this);
        }
    }

    float TilemapStream::progress() const {
        if (m_total_chunks <= 0) return 0.0f;
        return static_cast<float>(m_loaded_chunks) / m_total_chunks;
    }

    void TilemapStream::worker_thread() {
        int chunks_x = (m_width + m_chunk_size - 1) / m_chunk_size;
        int chunks_y = (m_height + m_chunk_size - 1) / m_chunk_size;

        for (int cy = 0; cy < chunks_y && m_running; cy++) {
            for (int cx = 0; cx < chunks_x && m_running; cx++) {
                TileChunk chunk = read_next_chunk();
                chunk.chunk_x = cx;
                chunk.chunk_y = cy;
                chunk.width = std::min(m_chunk_size, m_width - cx * m_chunk_size);
                chunk.height = std::min(m_chunk_size, m_height - cy * m_chunk_size);

                chunk.is_last = (cy == chunks_y - 1 && cx == chunks_x - 1);

                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    m_chunks.push(std::move(chunk));
                    m_loaded_chunks++;
                    m_cv.notify_one();
                }

                if (m_callback) {
                    TileChunk copy = chunk;
                    m_callback(copy);
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }

        m_has_more = false;

        TileChunk sentinel;
        sentinel.is_last = true;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_chunks.push(sentinel);
            m_cv.notify_one();
        }

        m_file.close();
    }

    void TilemapStream::parse_header() {
        std::string line;
        std::regex width_regex(R"(width\s*=\s*(\d+))");
        std::regex height_regex(R"(height\s*=\s*(\d+))");
        std::smatch match;

        while (std::getline(m_file, line)) {
            if (line.empty() || line[0] == '#') continue;

            if (std::regex_search(line, match, width_regex)) {
                m_width = std::stoi(match[1]);
                std::cout << "[TilemapStream] Width: " << m_width << std::endl;
            }
            else if (std::regex_search(line, match, height_regex)) {
                m_height = std::stoi(match[1]);
                std::cout << "[TilemapStream] Height: " << m_height << std::endl;
            }
            else if (line == "tilemap:") {
                break;
            }
        }
    }

    TileChunk TilemapStream::read_next_chunk() {
        TileChunk chunk;
        int current_chunk_height = 0;

        while (current_chunk_height < m_chunk_size && !m_file.eof()) {
            std::string line;
            std::getline(m_file, line);

            if (line.empty()) continue;
            if (line[0] == '#') continue;

            if ((int)line.length() < m_width) {
                line.append(m_width - line.length(), '.');
            }

            for (int x = 0; x < m_chunk_size && x < m_width; x++) {
                char c = line[x];
                chunk.tiles.push_back(char_to_tile(c));
            }

            current_chunk_height++;
        }

        chunk.height = current_chunk_height;
        if (chunk.tiles.empty()) {
            chunk.is_last = true;
        }

        return chunk;
    }

    int TilemapStream::char_to_tile(char c) const {
        switch (c) {
        case '#': return 1;  
        case '@': return 2;  
        case '~': return 3;   
        case '*': return 4;  
        case '+': return 5;   
        case '.': return 0;   
        default:  return 0;
        }
    }

    StreamingTilemapBuilder::StreamingTilemapBuilder(int tile_size, int chunk_size)
        : m_tile_size(tile_size)
        , m_chunk_size(chunk_size)
        , m_total_tiles(0)
        , m_processed_tiles(0)
        , m_progress(0.0f)
    {
    }

    StreamingTilemapBuilder::~StreamingTilemapBuilder() = default;

    void StreamingTilemapBuilder::build(TilemapStream& stream, TileHandler handler) {
        m_total_tiles = stream.get_width() * stream.get_height();
        m_processed_tiles = 0;

        while (stream.has_next()) {
            TileChunk chunk = stream.next();

            if (chunk.is_error) {
                std::cerr << "[TilemapBuilder] Error: " << chunk.error_msg << std::endl;
                break;
            }

            if (chunk.is_last) break;

            for (int y = 0; y < chunk.height; y++) {
                for (int x = 0; x < chunk.width; x++) {
                    int global_x = chunk.chunk_x * m_chunk_size + x;
                    int global_y = chunk.chunk_y * m_chunk_size + y;
                    int tile_id = chunk.get_tile(x, y);

                    handler(global_x, global_y, tile_id);
                    m_processed_tiles++;
                }
            }

            m_progress = static_cast<float>(m_processed_tiles) / m_total_tiles;
        }
    }
} 