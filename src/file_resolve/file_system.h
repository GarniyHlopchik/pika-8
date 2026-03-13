#pragma once
#include "globals.h"
#include <filesystem>
#include "file_resolve/zip.h"


class FileSystem{
    public:
    static void init(EngineReadState state, const std::string& path);
    static Resource get_resource(const std::string& relative_path);
    static void shutdown();
    private:
    static EngineReadState data_source;
    static std::string root_path;

};