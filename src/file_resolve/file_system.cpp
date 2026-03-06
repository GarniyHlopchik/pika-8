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
    if(data_source==EngineReadState::ZIP){
        return Zip::load(relative_path);
    }else if(data_source==EngineReadState::DIRECTORY){
        std::filesystem::path base = root_path;
        std::filesystem::path file = relative_path;
        std::filesystem::path full = base / file;
        return load_resource_from_directory(full.string());
    }
}
void FileSystem::shutdown(){
    if(data_source==EngineReadState::ZIP){
        Zip::shutdown();
    }
}