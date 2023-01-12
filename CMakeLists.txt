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

# ============================================================================
# Game Engine configuration
# ============================================================================


add_library(game_engine
	src/game_engine/command/EntityWaitCommand.cc
	src/game_engine/command/EntityMoveCommand.cc
	src/game_engine/entity/Entity.cc
	src/game_engine/entity/EntityStep.cc
	src/game_engine/entity/Vector.cc
	src/game_engine/step/ConflictPositionSolver.cc
	src/game_engine/step/Step.cc
	src/game_engine/graph/Graph.cc
	src/game_engine/graph/Grid.cc
	src/game_engine/graph/GridNode.cc
)

target_link_libraries(game_engine PRIVATE Boost::graph)

# ============================================================================
# Game configuration
# ============================================================================

target_include_directories(game_engine
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/game_engine/>
)

add_executable(game src/main.cc)

target_link_libraries(game PRIVATE game_engine)

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
