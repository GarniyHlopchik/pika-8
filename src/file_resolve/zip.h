#include "config.h"

class Zip {
private:
    static mz_zip_archive s_archive;
    static bool s_is_initialized;

    // Private constructor prevents instantiation
    Zip() = default;

public:
    static bool init(const std::string& zip_path);

    static Resource load(const std::string& internal_path);

    static void shutdown();
};