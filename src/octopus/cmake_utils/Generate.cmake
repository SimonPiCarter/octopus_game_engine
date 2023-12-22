

# ============================================================================
# Version management
# ============================================================================

find_package(Git)

if(Git_FOUND)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
		RESULT_VARIABLE result
		OUTPUT_VARIABLE GIT_HASH
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
else()
	set(result 1)
endif()

if(result)
	set(GIT_HASH ${PROJECT_VERSION})
endif()

MESSAGE(STATUS "Updated version =\"${GIT_HASH}\"")

configure_file(Version.cc.in Version.cc @ONLY)
