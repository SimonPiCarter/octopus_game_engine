# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

# ============================================================================
# Levels configuration
# ============================================================================

add_library(levels
	${CMAKE_CURRENT_SOURCE_DIR}/src/cases/Case1.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/cases/Case2.cc
)

target_link_libraries(levels PUBLIC octopus)

target_include_directories(levels
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
)
