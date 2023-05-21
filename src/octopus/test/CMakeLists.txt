# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

# ============================================================================
# External requirements
# ============================================================================

find_package (GTest REQUIRED)

# ============================================================================
# Targets
# ============================================================================

# ----------------------------------------------------------------------------
# unit_tests
# ----------------------------------------------------------------------------

add_executable (unit_tests)

target_sources (unit_tests
	PRIVATE
	src/areaSpawnerCommand.test.cc
	src/attackCommand.test.cc
	src/attackMoveCommand.test.cc
	src/autoAttackCommand.test.cc
	src/autoBuffCommand.test.cc
	src/benchmark.test.cc
	src/bitset.test.cc
	src/buffCommand.test.cc
	src/buffStep.test.cc
	src/buildingCommand.test.cc
	src/canonical.test.cc
	src/commandQueue.test.cc
	src/commandSerialization.test.cc
	src/conflictSolver.test.cc
	src/constraintPosition.test.cc
	src/controller.test.cc
	src/entityHitPointChange.test.cc
	src/entityMoveStep.test.cc
	src/entitySpawnStep.test.cc
	src/fixed.test.cc
	src/flyingCommant.test.cc
	src/grid.test.cc
	src/harvestCommand.test
	src/moveCommand.test.cc
	src/playerBuffAllStep.test.cc
	src/queueCommand.test.cc
	src/rallyPointProductionCommand.test.cc
	src/random.test.cc
	src/resourceSpentSimultaneous.test.cc
	src/spawningGrid.test.cc
	src/stateDrawStep.test.cc
	src/stateWinStep.test.cc
	src/stepOptionsGenerator.test.cc
	src/triggerBuilding.test.cc
	src/triggerTime.test.cc
	src/triggerUnitDied.test.cc
	src/triggerUnitSpawn.test.cc
	src/unitProductionCommand.test.cc
	src/upgradeProductionCommand.test.cc
	src/visionGridMoveStep.test.cc
	src/visionGridOnStep.test.cc
)

target_link_libraries (unit_tests
	PRIVATE
	GTest::gtest_main
	octopus
)
