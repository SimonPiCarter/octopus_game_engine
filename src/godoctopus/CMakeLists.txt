cmake_minimum_required(VERSION 3.19)
project(gdextension)

include(FetchContent)

FetchContent_Declare(
        GDExtension
        GIT_REPOSITORY https://github.com/godotengine/godot-cpp.git
        GIT_TAG 4.0
)

FetchContent_MakeAvailable(GDExtension)

# Automatically pick up files added to src
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Add a dynamic library called gameplay - this builds gameplay.dll
add_library(godoctopus SHARED ${SOURCES})

target_include_directories(godoctopus PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(godoctopus PUBLIC godot::cpp fair_and_square_lib)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX src FILES ${SOURCES})
