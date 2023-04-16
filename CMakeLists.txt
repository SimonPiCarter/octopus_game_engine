# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

# ============================================================================
# Project configuration
# ============================================================================

project (game
	VERSION 0.1.0
	LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ===========================================================
# External requirements
# ===========================================================

# ----------------------------------------------------------------------------
# Optional conan setup
# ----------------------------------------------------------------------------
if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	include (${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup (#[[TARGETS]])
endif ()

# ----------------------------------------------------------------------------
# External requirements setup
# ----------------------------------------------------------------------------
# additional cmake modules
list (PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list (PREPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

# library behaviour
set (CMAKE_FIND_PACKAGE_SORT_ORDER NATURAL)
set (CMAKE_FIND_PACKAGE_SORT_DIRECTION DEC)
set (CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)

find_package(Boost 1.75.0 REQUIRED)
add_definitions(-DBOOST_ALLOW_DEPRECATED_HEADERS)

# ============================================================================
# Game Engine configuration
# ============================================================================

add_subdirectory(src/octopus)

if(GRAPHICS)
	add_subdirectory(src/cuttlefish)
endif()

add_subdirectory(src/levels)
add_subdirectory(src/game)
add_subdirectory(src/fair_and_square)
if(GODOT)
	add_subdirectory(src/godoctopus)
endif()

# ============================================================================
# Installation
# ============================================================================

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install/${PROJECT_NAME}-${PROJECT_VERSION}" CACHE PATH "${PROJECT_NAME} default install path" FORCE)
endif ()

install (TARGETS game EXPORT ${PROJECT_NAME}-export)

set (ConfigPackageLocation "lib/cmake/${PROJECT_NAME}-${PROJECT_VERSION}")

install (EXPORT ${PROJECT_NAME}-export
	FILE ${PROJECT_NAME}-targets.cmake
	NAMESPACE ${PROJECT_NAME}::
	DESTINATION "${ConfigPackageLocation}"
	)
