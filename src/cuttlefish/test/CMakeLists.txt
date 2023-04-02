# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

# ===================
# test_wrapped_text
# ===================

add_executable(test_wrapped_text
${CMAKE_CURRENT_SOURCE_DIR}/src/test_wrapped_text.cc
)

target_link_libraries(test_wrapped_text PUBLIC cuttlefish)

# ===================
# Test_panel
# ===================

add_executable(test_panel
${CMAKE_CURRENT_SOURCE_DIR}/src/test_panel.cc
)

target_link_libraries(test_panel PUBLIC cuttlefish)

# ===================
# Test_panel
# ===================

add_executable(test_progress_bar
${CMAKE_CURRENT_SOURCE_DIR}/src/test_progress_bar.cc
)

target_link_libraries(test_progress_bar PUBLIC cuttlefish)

# ===================
# Test_logical_center
# ===================

add_executable(test_logical_center
${CMAKE_CURRENT_SOURCE_DIR}/src/test_logical_center.cc
)

target_link_libraries(test_logical_center PUBLIC cuttlefish)
