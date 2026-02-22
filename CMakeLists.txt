cmake_minimum_required(VERSION 4.0.0)
project(pika-8 VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(pika-8 src/config.h src/main.cpp dependencies/glad/glad.c 
src/sprite_mesh/mesh.h src/sprite_mesh/mesh.cpp src/shader_utils/shader_utils.h
src/shader_utils/shader_utils.cpp src/lua_system/lua_system.cpp src/lua_system/lua_system.h 
src/gfx/gfx.h src/gfx/gfx.cpp dependencies/stb_image/stb_image.h src/json_reader/json_reader.cpp src/user_input/user_input.cpp
dependencies/miniaudio/miniaudio.h src/sfx/sfx.h src/sfx/sfx.cpp src/scene_tree/node.cpp src/scene_tree/node.h)

target_include_directories(pika-8 PRIVATE dependencies)


include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

target_link_libraries(pika-8 PRIVATE glfw)

FetchContent_Declare(
    simdjson
    GIT_REPOSITORY https://github.com/simdjson/simdjson.git
    GIT_TAG v3.2.0
)
FetchContent_MakeAvailable(simdjson)

target_link_libraries(pika-8 PRIVATE simdjson::simdjson)

find_package(OpenGL REQUIRED)
target_link_libraries(pika-8 PRIVATE OpenGL::GL)

file(GLOB LUA_SOURCES
    dependencies/lua/*.c
)

# Exclude Lua files that define `main()` (lua and luac are standalone programs)
list(REMOVE_ITEM LUA_SOURCES
    ${CMAKE_SOURCE_DIR}/dependencies/lua/lua.c
    ${CMAKE_SOURCE_DIR}/dependencies/lua/luac.c
)

add_library(lua STATIC ${LUA_SOURCES})

if (MSVC)
    set_property(TARGET lua PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
    )
endif()

target_include_directories(lua PUBLIC dependencies/lua)
target_link_libraries(pika-8 PRIVATE lua)

target_include_directories(pika-8 PRIVATE
    src
    src/data_types
    dependencies/glad
)

if (WIN32)
    target_link_libraries(pika-8 PRIVATE opengl32)
endif()
if (MSVC)
    set_property(TARGET pika-8 PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
    )
endif()
if (UNIX AND NOT APPLE)
    target_link_libraries(pika-8 PRIVATE dl pthread)
endif()
