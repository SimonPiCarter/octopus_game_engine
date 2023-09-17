
# routine to make it update
EXECUTE_PROCESS(COMMAND ${CMAKE_BINARY_DIR}/RetreiveDateTime OUTPUT_VARIABLE DATETIMESTRING)
MESSAGE(STATUS "DATETIME=\"${DATETIMESTRING}\"")

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

configure_file(Version.cc.in Version.cc @ONLY)
