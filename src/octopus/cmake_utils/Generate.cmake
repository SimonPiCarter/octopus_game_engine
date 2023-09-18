

# ============================================================================
# Version management
# ============================================================================

find_package(Git REQUIRED)
execute_process(
	COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
	RESULT_VARIABLE result
	OUTPUT_VARIABLE GIT_HASH
	OUTPUT_STRIP_TRAILING_WHITESPACE
	)

if(result)
    message(FATAL_ERROR "Failed to get git hash: ${result}")
endif()

MESSAGE(STATUS "Updated version =\"${GIT_HASH}\"")

configure_file(Version.cc.in Version.cc @ONLY)
