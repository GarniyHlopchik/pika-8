#pragma once
#include <future>
#include <functional>
#include <queue>
#include <mutex>

class AsyncLoader {
public:
    // run a task asynchronously and return a future
    template<typename F, typename... Args>
    static auto run_async(F&& func, Args&&... args) {
        return std::async(std::launch::async, std::forward<F>(func), std::forward<Args>(args)...);
    }

    // queue a function to run on the main thread (call this in your main loop)
    static void run_on_main_thread(std::function<void()> func) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        main_thread_tasks.push(func);
    }

    // process all queued main-thread tasks (call this in your main loop)
    static void process_main_thread_tasks() {
        std::lock_guard<std::mutex> lock(queue_mutex);
        while (!main_thread_tasks.empty()) {
            auto task = main_thread_tasks.front();
            main_thread_tasks.pop();
            task();
        }
    }

private:
    static std::mutex queue_mutex;
    static std::queue<std::function<void()>> main_thread_tasks;
};

inline std::mutex AsyncLoader::queue_mutex;
inline std::queue<std::function<void()>> AsyncLoader::main_thread_tasks;