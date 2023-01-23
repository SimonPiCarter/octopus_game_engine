# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

# ============================================================================
# Game Engine configuration
# ============================================================================


add_library(octopus
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/BuildingUnitProductionCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/Command.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/Commandable.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/CommandQueue.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/EntityAttackCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/EntityAttackMoveCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/EntityBuffCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/EntityWaitCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/EntityMoveCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/UnitHarvestCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/logger/Logger.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/controller/Controller.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/controller/Metrics.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/entity/Buff.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/entity/Building.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/entity/Entity.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/entity/Resource.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/entity/Unit.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/State.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/ConflictPositionSolver.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/Step.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/TickingStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/command/CommandDataWaypointStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/command/CommandIncrementNoProgressStep
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/command/CommandMoveUpdateStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/command/CommandNewTargetStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/command/CommandQueueStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/command/CommandSetPositionFromStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/command/CommandSubAttackStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/command/CommandUpdateLastPosStep
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/command/CommandWindUpDiffStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/command/ProductionProgressionStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/command/harvest/CommandHarvestStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/building/BuildingStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/EntityAttackStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/EntityHitPointChangeStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/EntityMoveStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/buff/EntityBuffStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/spawn/EntitySpawnStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/spawn/UnitSpawnStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/spawn/BuildingSpawnStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/spawn/ResourceSpawnStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/player/PlayerSpawnStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/unit/UnitHarvestStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/graph/Graph.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/graph/Grid.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/graph/GridNode.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/Library.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/utils/Vector.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/utils/DynamicBitset.cc
)

target_link_libraries(octopus PRIVATE Boost::graph)

target_include_directories(octopus
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
)

add_subdirectory(test)
