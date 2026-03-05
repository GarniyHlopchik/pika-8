#ifndef ENGINE_GLOBALS
#define ENGINE_GLOBALS

enum class EngineReadState { // the way engine looks for files: 
    DIRECTORY, //just in a folder
    ZIP,       //in a .zip archive 
    EMBEDDED,  //zip appended to the end of the executable
    UNDEFINED  //not defined yet, fall back to directory on the current directory
};
struct Resource {
    // Custom deleter to ensure miniz heap memory is freed correctly
    std::unique_ptr<uint8_t[], void(*)(void*)> data{nullptr, [](void* p) { mz_free(p); }};
    size_t size = 0;

    // Helper to check if the resource is valid
    bool is_valid() const { return data != nullptr; }
};
class LuaSystem;
class SceneTree;
class GFX;
class SFX;
class Config;
struct EngineContext {
    LuaSystem* lua;
    SceneTree* scene_tree;
    GFX* gfx;
    SFX* sfx;
    Config* config;
};
#endif