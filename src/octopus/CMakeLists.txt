# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

# ============================================================================
# Game Engine configuration
# ============================================================================


add_library(octopus
	${CMAKE_CURRENT_SOURCE_DIR}/command/EntityWaitCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/command/EntityMoveCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/state/entity/Entity.cc
	${CMAKE_CURRENT_SOURCE_DIR}/step/ConflictPositionSolver.cc
	${CMAKE_CURRENT_SOURCE_DIR}/step/Step.cc
	${CMAKE_CURRENT_SOURCE_DIR}/step/entity/EntityStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/graph/Graph.cc
	${CMAKE_CURRENT_SOURCE_DIR}/graph/Grid.cc
	${CMAKE_CURRENT_SOURCE_DIR}/graph/GridNode.cc
	${CMAKE_CURRENT_SOURCE_DIR}/utils/Vector.cc
)

target_link_libraries(octopus PRIVATE Boost::graph)

target_include_directories(octopus
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
