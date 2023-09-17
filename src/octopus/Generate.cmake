
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

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/utils/Version.cc.in ${CMAKE_BINARY_DIR}/Version.cc @ONLY)