# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

# ============================================================================
# Levels configuration
# ============================================================================

add_library(fair_and_square_lib
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/AnchorDivinityTrigger.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/AnchorTrigger.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/ModelLoader.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/TimerDamage.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/bonus/BuffGenerators.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/divinity/DivinityGenerator.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/divinity/DivinityModelLoader.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/divinity/generators/AttackSpeedDivinityStepGenerator.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/divinity/generators/EconomicDivinityStepGenerator.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/divinity/generators/HealDivinityStepGenerator.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/divinity/generators/LifestealDivinityStepGenerator.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/divinity/generators/ProductionDivinityStepGenerator.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/model/divinity/generators/RecycleDivinityStepGenerator.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/levels/ArenaLevel.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/levels/LevelId.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/levels/MazeLevel.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/levels/WaveLevel.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/levels/showcase/AoeShowcase.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/levels/showcase/ChainingShowcase.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/levels/showcase/DotShowcase.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/levels/showcase/LifestealShowcase.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/utils/LoseTrigger.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/utils/Randomizer.cc
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
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/game/loader/trigger/AnchorEffectTrigger.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/game/loader/visitor/FASStepVisitor.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/game/RunGame.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/game/RunLevel1.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/game/RunnerHelpers.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/game/showcase/RunShowcase.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/core/game/level1/Level1.cc
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

# ============================================================================
# Installation
# ============================================================================

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install/${PROJECT_NAME}-${PROJECT_VERSION}" CACHE PATH "${PROJECT_NAME} default install path" FORCE)
endif ()

install (TARGETS fair_and_square_lib EXPORT fair_and_square_lib-export)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/" # source directory
        DESTINATION "include/fair_and_square/" # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.hh" # select header files
)

set (ConfigPackageLocation "lib/cmake/${PROJECT_NAME}-${PROJECT_VERSION}")
