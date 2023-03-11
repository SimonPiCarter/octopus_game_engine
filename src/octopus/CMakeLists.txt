# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

# ============================================================================
# Game Engine configuration
# ============================================================================


add_library(octopus
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/building/BuildingBlueprintCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/building/BuildingUnitProductionCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/Command.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/Commandable.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/CommandQueue.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/data/AttackMoveData.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/entity/EntityAttackCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/entity/EntityAttackMoveCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/entity/EntityBuffCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/entity/EntityBuildingCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/entity/EntityMoveCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/entity/EntityWaitCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/player/PlayerChoseDivinityCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/spawner/AreaSpawnerCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/spawner/SpawningGrid.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/unit/UnitDropCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/command/unit/UnitHarvestCommand.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/logger/Logger.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/orca/OrcaManager.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/orca/rvo/Agent.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/orca/rvo/KdTree.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/orca/rvo/Obstacle.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/orca/rvo/RVOSimulator.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/controller/Controller.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/controller/Metrics.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/controller/event/EventCollection.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/controller/trigger/Listener.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/controller/trigger/Trigger.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/entity/Buff.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/entity/Building.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/entity/Entity.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/entity/Resource.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/entity/Unit.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/model/requirements/Requirements.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/model/entity/BuildingModel.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/model/entity/EntityModel.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/model/entity/TempleModel.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/player/Player.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/vision/VisionGrid.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/vision/VisionHandler.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/ResourceType.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/state/State.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/ConflictPositionSolver.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/Step.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/StepAdditionVisitor.cc
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
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/command/ProductionPaidStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/command/ProductionProgressionStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/command/harvest/CommandHarvestStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/building/BuildingCancelStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/building/BuildingStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/EntityAttackStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/EntityFrozenStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/EntityHitPointChangeStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/EntityMoveStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/EntityUpdateWaitingStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/buff/EntityBuffStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/spawn/EntitySpawnStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/spawn/UnitSpawnStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/spawn/BuildingSpawnStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/entity/spawn/ResourceSpawnStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/player/PlayerAddOptionDivinityStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/player/PlayerAnchorDivinityStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/player/PlayerLevelUpDivinityStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/player/PlayerResetOptionDivinityStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/player/PlayerSpawnStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/player/PlayerSpendResourceStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/state/StateDrawStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/state/StateTemplePositionAddStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/state/StateTemplePositionRemoveStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/state/StateWinStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/trigger/TriggerCountChange.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/trigger/TriggerEnableChange.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/trigger/TriggerEntityAddStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/trigger/TriggerEntityResetStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/trigger/TriggerSpawn.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/trigger/TriggerStepCountChange.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/step/unit/UnitHarvestStep.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/graph/CanonicalDijkstra.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/graph/Graph.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/graph/Grid.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/graph/GridNode.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/graph/PathManager.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/library/Library.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/utils/DynamicBitset.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/utils/Fixed.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/utils/Vector.cc
)

target_link_libraries(octopus PRIVATE Boost::graph)

target_include_directories(octopus
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
)

add_subdirectory(test)
