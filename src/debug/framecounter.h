#pragma once
#include <chrono>
#include <cstdint>

class FrameCounter {
public:
    static inline void init() {
        frame_count = 0;
    }

    static inline void add_frame() {
        frame_count++;
    }

    static inline uint64_t  get_frame_count() {
        return frame_count;
    }

    FrameCounter() = default;
private:
    static inline uint64_t  frame_count = 0; // C++17: inline definition + initializer
};