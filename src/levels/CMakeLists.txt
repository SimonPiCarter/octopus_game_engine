# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

# ============================================================================
# Levels configuration
# ============================================================================

add_library(levels
	${CMAKE_CURRENT_SOURCE_DIR}/src/cases/Case2.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/cases/Case3.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/cases/Case4.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/cases/Case5.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/DivinityArmorFiller.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/DivinityFireFiller.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/DivinityRaidFiller.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/DivinitySwarmFiller.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/NeutralFiller.cc
)

target_link_libraries(levels PUBLIC octopus)

target_include_directories(levels
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
)
