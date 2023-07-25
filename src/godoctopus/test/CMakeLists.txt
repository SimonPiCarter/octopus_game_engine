# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

target_sources (unit_tests
	PRIVATE
	src/level2HeaderIO.test.cc
)

target_link_libraries (unit_tests
	PRIVATE
	godoctopus_static
)
