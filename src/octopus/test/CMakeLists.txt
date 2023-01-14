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
	src/controller.test.cc
	src/entityMoveStep.test.cc
	src/entitySpawnStep.test.cc
	src/grid.test.cc
	src/moveCommand.test.cc
)

target_link_libraries (unit_tests
	PRIVATE
	GTest::gtest_main
	octopus
)
