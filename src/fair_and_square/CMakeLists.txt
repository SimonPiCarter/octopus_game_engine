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

target_link_libraries(fair_and_square_lib PUBLIC octopus)

target_include_directories(fair_and_square_lib
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
)

add_executable(fair_and_square_raw
		${CMAKE_CURRENT_SOURCE_DIR}/src/raw/raw.cc
	)

target_link_libraries(fair_and_square_raw PUBLIC fair_and_square_lib)

if(GRAPHICS)
	add_executable(fair_and_square
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/game/game.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/core.cc
	)
	target_link_libraries(fair_and_square PUBLIC fair_and_square_lib cuttlefish)
endif()
