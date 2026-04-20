cmake_minimum_required(VERSION 3.16)
project(pika-8 VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ---------------- SDL3 ----------------
if (WIN32)
    set(SDL_STATIC ON CACHE BOOL "Link SDL3 statically" FORCE)
    set(SDL_SHARED OFF CACHE BOOL "Link SDL3 shared" FORCE)
endif()



set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.2
)


if(ANDROID)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(SDL3)

# ---------------- simdjson ----------------

FetchContent_Declare(
    simdjson
    GIT_REPOSITORY https://github.com/simdjson/simdjson.git
    GIT_TAG v3.2.0
)

FetchContent_MakeAvailable(simdjson)

# ---------------- Lua ----------------

set(LUA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lua)

if(PIKA_USE_ONELUA)
    set(LUA_SOURCES ${LUA_DIR}/onelua.c)
else()
    file(GLOB LUA_SOURCES
        ${LUA_DIR}/*.c
    )

    list(REMOVE_ITEM LUA_SOURCES
        ${LUA_DIR}/lua.c
        ${LUA_DIR}/luac.c
        ${LUA_DIR}/onelua.c
    )
endif()

add_library(lua STATIC ${LUA_SOURCES})
target_include_directories(lua PUBLIC ${LUA_DIR})
target_compile_definitions(lua PRIVATE MAKE_LIB)

# ---------------- LuaSocket ----------------

set(LUASOCKET_SOURCES
    dependencies/luasocket/mime.c
    dependencies/luasocket/luasocket.c
    dependencies/luasocket/timeout.c
    dependencies/luasocket/buffer.c
    dependencies/luasocket/io.c
    dependencies/luasocket/select.c
    dependencies/luasocket/auxiliar.c
    dependencies/luasocket/options.c
    dependencies/luasocket/inet.c
    dependencies/luasocket/tcp.c
    dependencies/luasocket/udp.c
    dependencies/luasocket/except.c
    dependencies/luasocket/compat.c
)

if (WIN32)
    list(APPEND LUASOCKET_SOURCES dependencies/luasocket/wsocket.c)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
else()
    list(APPEND LUASOCKET_SOURCES dependencies/luasocket/usocket.c)
endif()

add_library(luasocket STATIC ${LUASOCKET_SOURCES})

target_include_directories(luasocket PUBLIC
    dependencies/lua
    dependencies/luasocket
)

# ---------------- Source Files ----------------

set(PIKA_SOURCES
    src/main.cpp
    dependencies/glad/glad.c
    src/sprite_mesh/mesh.cpp
    src/shader_utils/shader_utils.cpp
    src/lua_system/lua_system.cpp
    src/gfx/gfx.cpp
    src/debug/debug.cpp
    src/gfx/text/text.cpp
    src/gfx/sprite/sprite.cpp
    src/json_reader/json_reader.cpp
    src/user_input/user_input.cpp
    src/sfx/sfx.cpp
    src/scene_tree/node.cpp
    src/scene_tree/scene_tree.cpp
    src/scene_tree/node_factory.cpp
    dependencies/pugixml/pugixml.cpp
    dependencies/miniz/miniz.c
    src/lua_bindings/engine_context.cpp
    src/lua_bindings/lua_bind/lua_bind_gfx.cpp
    src/lua_bindings/lua_bind/lua_bind_gfx_sprite.cpp
    src/lua_bindings/lua_bind/lua_bind_input.cpp
    src/lua_bindings/lua_bind/lua_bind_sfx.cpp
    src/lua_bindings/lua_bind/lua_bind_node.cpp
    src/lua_bindings/table_getter/table_gatter.cpp
    src/file_resolve/file_system.cpp
    src/file_resolve/zip.cpp
    src/lua_bindings/sol_bind/sol_bind_node.cpp
    src/lua_bindings/sol_bind/sol_bind_sprite.cpp
    src/mobile_input/input_state.h
    src/scene_tree/node2d.cpp
    src/lua_bindings/sol_bind/sol_bind_node2d.cpp
    src/lua_bindings/lua_bind/lua_bind_input.cpp
    src/scene_tree/y_sort.cpp
    src/lua_bindings/sol_bind/sol_bind_ysort.cpp
)

# ---------------- Target ----------------

if(ANDROID)
    add_library(pika-8 SHARED ${PIKA_SOURCES})
else()
    add_executable(pika-8 ${PIKA_SOURCES}
)
endif()

# ---------------- Includes ----------------

target_include_directories(pika-8 PRIVATE
    src
    src/mobile_input 
    dependencies
    dependencies/lua
    dependencies/sol
    dependencies/glad
)

# ---------------- Link Libraries ----------------

target_link_libraries(pika-8 PRIVATE
    lua
    luasocket
    simdjson::simdjson
)

# ---------------- Platform specific ----------------

if(ANDROID)

    find_library(GLES_LIB GLESv3)
    find_library(LOG_LIB log)
    find_library(ANDROID_LIB android)

    target_link_libraries(pika-8 PRIVATE
    SDL3::SDL3
    ${GLES_LIB}
    ${LOG_LIB}
    ${ANDROID_LIB}
    )

    target_compile_definitions(pika-8 PRIVATE PIKA_GLES)

else()

    find_package(OpenGL REQUIRED)

    target_link_libraries(pika-8 PRIVATE
        SDL3::SDL3
        OpenGL::GL
    )

    if(WIN32)
        target_link_libraries(pika-8 PRIVATE
            opengl32
            ws2_32
        )
    endif()

    if(UNIX AND NOT APPLE)
        target_link_libraries(pika-8 PRIVATE
            dl
            pthread
            m
        )
    endif()

endif()

# ----------- Emscripten ------------------
if(EMSCRIPTEN)
    set_target_properties(pika-8 PROPERTIES SUFFIX ".html")
    target_compile_definitions(pika-8 PRIVATE PIKA_GLES)

    target_link_options(pika-8 PRIVATE 
        "-sMAX_WEBGL_VERSION=2" 
        "-sMIN_WEBGL_VERSION=2"
        "-sINITIAL_MEMORY=64MB"
        "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/shell.html"
    )
endif()