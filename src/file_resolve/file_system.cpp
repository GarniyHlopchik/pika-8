#define MINIZ_IMPLEMENTATION
#include "globals.h"
#include "file_system.h"
#include <fstream>
#include <filesystem>
#include "miniz/miniz.h"
#include <cstdlib>

Resource load_resource_from_directory(const std::filesystem::path& path) {
    Resource res;

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        return res;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    void* mem = malloc(size);
    if (!mem) {
        return res;
    }

    if (!file.read((char*)mem, size)) {
        mz_free(mem);
        return res;
    }

    res.data.reset((uint8_t*)mem);
    res.size = size;

    return res;
}

EngineReadState FileSystem::data_source = EngineReadState::UNDEFINED;
std::string FileSystem::root_path = "";

void FileSystem::init(EngineReadState state, const std::string& path){
    data_source = state;
    root_path = path;
    if(state==EngineReadState::ZIP){
        Zip::init(path);
    }
}
Resource FileSystem::get_resource(const std::string& relative_path){
    if(data_source==EngineReadState::ZIP)
    {
        return Zip::load(relative_path);
    }
    else if(data_source==EngineReadState::DIRECTORY)
    {
        std::filesystem::path base = root_path;
        std::filesystem::path file = relative_path;
        std::filesystem::path full = base / file;

        // Try to load the resource directly first
        Resource res = load_resource_from_directory(full.string());
        if (res.is_valid()) {
            return res;
        }

        // Fallback: some builds / runtimes place assets alongside the executable
        // (e.g., "fonts/" instead of "assets/fonts/"). If the requested
        // resource starts with "assets/", try loading without the prefix.
        const std::string assetsPrefixPosix = "assets/";
        const std::string assetsPrefixWin = "assets\\";

        auto tryStripPrefix = [&](const std::string& prefix) {
            if (relative_path.rfind(prefix, 0) != 0) return Resource{};
            std::filesystem::path stripped = relative_path.substr(prefix.size());
            std::filesystem::path p = base / stripped;
            return load_resource_from_directory(p.string());
        };

        if (Resource res2 = tryStripPrefix(assetsPrefixPosix); res2.is_valid()) {
            return res2;
        }
        if (Resource res3 = tryStripPrefix(assetsPrefixWin); res3.is_valid()) {
            return res3;
        }

        return res;
    }
    return Resource{}; // return empty if nothing found to prevent crash
}
void FileSystem::shutdown(){
    if(data_source==EngineReadState::ZIP){
        Zip::shutdown();
    }
}