#pragma once
#include "config.h"
#include <SDL3/SDL.h>

class Zip {
private:
    static mz_zip_archive s_archive;
    static bool s_is_initialized;
    static unsigned char* my_mem;
    // Private constructor prevents instantiation
    Zip() = default;

public:
    static bool init(const std::string& zip_path);

    static Resource load(const std::string& internal_path);

    static void shutdown();
};