# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

# ============================================================================
# Levels configuration
# ============================================================================

add_library(fair_and_square_lib
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/AnchorTrigger.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/ModelLoader.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/TimerDamage.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/bonus/BuffGenerators.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/levels/ArenaLevel.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/levels/MazeLevel.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/levels/WaveLevel.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/levels/showcase/AoeShowcase.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/levels/showcase/ChainingShowcase.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/levels/showcase/DotShowcase.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/levels/showcase/LifestealShowcase.cc
)

target_link_libraries(fair_and_square_lib PUBLIC octopus)

target_include_directories(fair_and_square_lib
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
)

add_executable(fair_and_square_raw
		${CMAKE_CURRENT_SOURCE_DIR}/src/raw/fair_and_square_raw.cc
	)

target_link_libraries(fair_and_square_raw PUBLIC fair_and_square_lib)

if(GRAPHICS)
	add_executable(fair_and_square
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/game/loader/ChoicePanel.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/game/loader/ResourceLoader.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/game/loader/choice/ChoiceSubPanel.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/game/RunGame.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/game/showcase/RunShowcase.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/lang/LangEntries.cc

		${CMAKE_CURRENT_SOURCE_DIR}/src/core/menu/MainMenu.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/menu/levels/ArenaMenu.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/menu/levels/MazeMenu.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/menu/levels/ShowcaseMenu.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/menu/levels/WaveMenu.cc

		${CMAKE_CURRENT_SOURCE_DIR}/src/core/fair_and_square_core.cc
	)
	target_link_libraries(fair_and_square PUBLIC fair_and_square_lib cuttlefish)
endif()
