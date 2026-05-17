#pragma once
#include <iostream>
#include <chrono>
#include <functional>
#include <utility>
#include "logger/proxy.h"
#include <string>
#include <new>

thread_local size_t active_allocations = 0;
thread_local size_t total_allocated_bytes = 0;

void* operator new(size_t size) {
    active_allocations++;
    total_allocated_bytes += size;
    return malloc(size);
}

void operator delete(void* memory) noexcept {
    if (active_allocations > 0) active_allocations--;
    free(memory);
}
void* operator new[](size_t size) {
    total_allocation_count++;
    total_allocated_bytes += size;
    return malloc(size);
}
void operator delete[](void* memory) noexcept {
    free(memory);
}

template <typename Func>
auto profile(Func f) {
    return [f](auto&&... args) {
        auto start = std::chrono::high_resolution_clock::now();
        size_t start_bytes = total_allocated_bytes;
        size_t start_allocations = active_allocations;

        auto result = std::invoke(f, std::forward<decltype(args)>(args)...);
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        size_t plus_bytes = total_allocated_bytes - start_bytes;
        size_t plus_allocations = active_allocations - start_allocations;


        
        LOG(LogLevel::DEBUG, "[Profiling] time elapsed: "+std::to_string(elapsed.count()));
        LOG(LogLevel::DEBUG, "[Profiling] new bytes allocated: "+std::to_string(plus_bytes));
        LOG(LogLevel::DEBUG, "[Profiling] new allocations : "+std::to_string(plus_allocations));
        
        return result;
    };
}