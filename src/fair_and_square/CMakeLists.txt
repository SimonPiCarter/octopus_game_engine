# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

# ============================================================================
# Levels configuration
# ============================================================================

add_library(fair_and_square_lib
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/wave_level/ModelLoader.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/wave_level/WaveLevel.cc
)

add_executable(fair_and_square
	${CMAKE_CURRENT_SOURCE_DIR}/src/core/game/game.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/core/core.cc
)

target_link_libraries(fair_and_square_lib PUBLIC cuttlefish)
target_link_libraries(fair_and_square PUBLIC fair_and_square_lib)

target_include_directories(fair_and_square_lib
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
)
