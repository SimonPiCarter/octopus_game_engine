# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

# ===================
# Test
# ===================

#add_executable(test
#${CMAKE_CURRENT_SOURCE_DIR}/src/test.cc
#)

#target_link_libraries(test PUBLIC cuttlefish)

# ===================
# Test_command
# ===================

#add_executable(test_command
#${CMAKE_CURRENT_SOURCE_DIR}/src/test_command.cc
#)

#target_link_libraries(test_command PUBLIC cuttlefish)

# ===================
# Test_panel
# ===================

add_executable(test_panel
${CMAKE_CURRENT_SOURCE_DIR}/src/test_panel.cc
)

target_link_libraries(test_panel PUBLIC cuttlefish)
