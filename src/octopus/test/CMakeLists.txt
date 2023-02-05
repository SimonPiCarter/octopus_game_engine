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
	src/attackCommand.test.cc
	src/attackMoveCommand.test.cc
	src/bitset.test.cc
	src/buffCommand.test.cc
	src/buffStep.test.cc
	src/buildingCommand.test.cc
	src/commandQueue.test.cc
	src/conflictSolver.test.cc
	src/controller.test.cc
	src/entityMoveStep.test.cc
	src/entitySpawnStep.test.cc
	src/grid.test.cc
	src/harvestCommand.test
	src/moveCommand.test.cc
	src/queueCommand.test.cc
	src/triggerBuilding.test.cc
	src/triggerTime.test.cc
	src/triggerUnitSpawn.test.cc
	src/unitProductionCommand.test.cc
)

target_link_libraries (unit_tests
	PRIVATE
	GTest::gtest_main
	octopus
)
