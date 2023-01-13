# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

# ============================================================================
# Game Engine configuration
# ============================================================================


add_library(octopus
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/Command.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/Commandable.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/EntityWaitCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/EntityMoveCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/logger/Logger.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/controller/Controller.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/State.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/entity/Entity.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/ConflictPositionSolver.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/Step.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/EntityHitPointChangeStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/EntityMoveStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/EntitySpawnStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/graph/Graph.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/graph/Grid.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/graph/GridNode.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/utils/Vector.cc
)

target_link_libraries(octopus PRIVATE Boost::graph)

target_include_directories(octopus
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
)

add_subdirectory(test)
