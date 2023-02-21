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

add_library(cuttlefish
	${CMAKE_CURRENT_SOURCE_DIR}/src/clicMode/BuildClicMode.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/clicMode/StandardClicMode.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/panel/DescPanel.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/panel/DivinityPanel.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/panel/Panel.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/sprite/Sprite.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/sprite/SpriteLibrary.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/text/Text.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/text/SegmentedText.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/text/WrappedText.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/texture/RenderTexture.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/texture/Texture.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/tilemap/Tilemap.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/window/Window.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/world/Selection.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/world/World.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/world/WorldUpdaterStepVisitor.cc
)

target_link_libraries(cuttlefish PUBLIC levels SDL2::SDL2main SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)

target_include_directories(cuttlefish
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
)

add_subdirectory(test)
