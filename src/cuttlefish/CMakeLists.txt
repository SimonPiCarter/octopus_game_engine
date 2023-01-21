# ============================================================================
# CMake configuration
# ============================================================================

cmake_minimum_required (VERSION 3.14.0)

# ============================================================================
# Render Engine configuration
# ============================================================================

find_package(SDL2 2.26.1 REQUIRED)
find_package(SDL2_image 2.0.5 REQUIRED)
find_package(SDL2_ttf 2.0.18 REQUIRED)

add_executable(cuttlefish
	${CMAKE_CURRENT_SOURCE_DIR}/src/sprite/Sprite.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/texture/Texture.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/window/Window.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/test.cc
)

target_link_libraries(cuttlefish PUBLIC octopus SDL2::SDL2main SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)

target_include_directories(cuttlefish
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
)