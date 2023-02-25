# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

# ============================================================================
# Levels configuration
# ============================================================================

add_executable(fair_and_square
	${CMAKE_CURRENT_SOURCE_DIR}/src/core/level1/level1.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/core/test_level1.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/wave_level/ModelLoader.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/wave_level/WaveLevel.cc
)

target_link_libraries(fair_and_square PUBLIC cuttlefish)

target_include_directories(fair_and_square
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
)
