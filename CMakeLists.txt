cmake_minimum_required(VERSION 4.0.0)
project(pika-8 VERSION 1.0.0 LANGUAGES C CXX)

add_executable(pika-8 src/config.h src/main.cpp dependencies/glad/glad.c)

target_include_directories(pika-8 PRIVATE dependencies)


include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

target_link_libraries(pika-8 PRIVATE glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(pika-8 PRIVATE OpenGL::GL)

file(GLOB LUA_SOURCES
    dependencies/lua/*.c
)

add_library(lua STATIC ${LUA_SOURCES})

target_include_directories(lua PUBLIC dependencies/lua)
target_link_libraries(pika-8 PRIVATE lua)

target_include_directories(pika-8 PRIVATE
    src
    dependencies/glad
)

if (WIN32)
    target_link_libraries(pika-8 PRIVATE opengl32)
endif()
if (MSVC)
    set_property(TARGET pika-8 PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded"
    )
endif()
if (UNIX AND NOT APPLE)
    target_link_libraries(pika-8 PRIVATE dl pthread)
endif()