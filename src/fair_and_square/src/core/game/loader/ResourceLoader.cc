#include "ResourceLoader.hh"

#include "window/Window.hh"
#include "world/World.hh"

namespace fas
{
RessourceLoader::RessourceLoader(cuttlefish::Window &window_p, unsigned long fullWorldSize_p, cuttlefish::World &world_p)
	: _panel(&window_p, window_p.getWidth()-276, window_p.getHeight()-276,
		window_p.loadTexture("resources/fair_and_square/panel.png"), window_p.loadTexture("resources/fair_and_square/grid.png"), 4)
	, _statsPanel(&window_p, window_p.getWidth()-276*2-2, window_p.getHeight()-276,
		window_p.loadTexture("resources/fair_and_square/panel.png"), window_p.loadTexture("resources/fair_and_square/grid.png"),
		window_p.loadTexture("resources/fair_and_square/prod_bar_back.png"), window_p.loadTexture("resources/fair_and_square/prod_bar_fill.png"), 4, world_p.getSelection())
	, _choicePanel(window_p, window_p.getWidth()-window_p.getWidth()/2-200, window_p.getHeight()-window_p.getHeight()/2-200,
		window_p.loadTexture("resources/fair_and_square/panel.png"), window_p.loadTexture("resources/fair_and_square/grid.png"), 0)
	, _descPanel(&window_p, window_p.getWidth()-260, window_p.getHeight()-520,
		window_p.loadTexture("resources/fair_and_square/panel.png"))
	, _spriteLib()
	, _tilemap(fullWorldSize_p, _spriteLib, "tiles", "details")
	, _minimap(window_p, 0, window_p.getHeight()-250, 250, 250, {"resources/me.png", "resources/enemy.png", "resources/ally.png"})
{
	_panel.addSpriteInfo("worker", 2, 1);
	_panel.addSpriteInfo("command_center", 1, 0);
	_panel.addSpriteInfo("barrack", 0, 0);
	_panel.addSpriteInfo("deposit", 0, 1);
	_panel.addSpriteInfo("anchor", 1, 1);
	_panel.addSpriteInfo("anchor_spot", 1, 2);

	_panel.addSpriteInfo("square", 1, 4);
	_panel.addSpriteInfo("triangle", 2, 3);
	_panel.addSpriteInfo("circle", 1, 3);

	// resources
	_statsPanel.addSpriteInfo("resource_steel", 2, 4);
	_statsPanel.addSpriteInfo("resource_food", 2, 4);

	_statsPanel.addSpriteInfo("worker", 2, 1);
	_statsPanel.addSpriteInfo("command_center", 1, 0);
	_statsPanel.addSpriteInfo("barrack", 0, 0);
	_statsPanel.addSpriteInfo("deposit", 0, 1);
	_statsPanel.addSpriteInfo("anchor", 1, 1);
	_statsPanel.addSpriteInfo("anchor_spot", 1, 2);

	_statsPanel.addSpriteInfo("square", 1, 4);
	_statsPanel.addSpriteInfo("triangle", 2, 3);
	_statsPanel.addSpriteInfo("circle", 1, 3);

	// _choicePanel.addOptionInfo(octopus::DivinityType::Divinity_1, 2, 0);
	// _choicePanel.addOptionInfo(octopus::DivinityType::Divinity_3, 1, 3);
	// _choicePanel.addOptionInfo(octopus::DivinityType::Divinity_4, 1, 4);
	// _choicePanel.addOptionInfo(octopus::DivinityType::Divinity_5, 2, 3);

	_descPanel.setText("ceci est un texte de test avec un parametre param $param");

	// hp bar
	_spriteLib.setHpBarBackground(window_p.loadTexture("resources/fair_and_square/hp_bar_back.png"));
	_spriteLib.setHpBarFilling(window_p.loadTexture("resources/fair_and_square/hp_bar_fill.png"));
	// tile map
	_spriteLib.registerSpriteTemplate("tiles", window_p.loadTexture("resources/fair_and_square/tiles.png"), 1., 0, 0, 64, 64, std::vector<int>(16, 1), std::vector<double>(16, 1), 1);
	_spriteLib.registerSpriteTemplate("details", window_p.loadTexture("resources/fair_and_square/details.png"), 0.5, 0, 0, 64, 64, {1}, {2}, 1);

	// resources
	_spriteLib.registerSpriteTemplate("resource_steel", window_p.loadTexture("resources/steel_prop1.png"), 2., 128, 128, 256, 256, {10, 10}, {0.2,0.2}, 1);
	_spriteLib.registerSpriteTemplate("resource_food", window_p.loadTexture("resources/fair_and_square/food_prop2.png"), 1., 32, 32, 64, 64, {14, 1}, {0.1, 1.}, 1);

	// neutral
	_spriteLib.registerSpriteTemplate("command_center", window_p.loadTexture("resources/fair_and_square/command_center.png"), 2., 64, 64, 128, 128, {8, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);
	_spriteLib.registerSpriteTemplate("barrack", window_p.loadTexture("resources/fair_and_square/barrack.png"), 1., 32, 32, 64, 64, {16, 1, 1, 1}, {0.25, 0.5, 0.5, 0.5}, 1);
	_spriteLib.registerSpriteTemplate("deposit", window_p.loadTexture("resources/fair_and_square/deposit.png"), 1., 32, 32, 64, 64, {14, 1, 1, 1}, {0.1, 0.5, 0.5, 0.5}, 1);
	_spriteLib.registerSpriteTemplate("anchor", window_p.loadTexture("resources/fair_and_square/deposit.png"), 1., 32, 32, 64, 64, {14, 1, 1, 1}, {0.1, 0.5, 0.5, 0.5}, 1);
	_spriteLib.registerSpriteTemplate("anchor_spot", window_p.loadTexture("resources/fair_and_square/anchor_spot.png"), 1., 32, 32, 64, 64, {1}, {0.1}, 1);
	_spriteLib.registerSpriteTemplate("worker", window_p.loadTexture("resources/fair_and_square/worker_v2.png"), 0.5, 32, 32, 64, 64, {2, 2}, {0.5, 0.1}, 1);
	_spriteLib.registerAltTextureForTemplate("worker", 0, window_p.loadTexture("resources/fair_and_square/worker_v2_green.png"));
	_spriteLib.registerAltTextureForTemplate("worker", 1, window_p.loadTexture("resources/fair_and_square/worker_v2_red.png"));

	_spriteLib.registerSpriteTemplate("square", window_p.loadTexture("resources/fair_and_square/square.png"), 0.5, 16, 16, 32, 32, {8, 8, 8}, {0.25, 0.5/7., 0.05}, 1);
	_spriteLib.registerAltTextureForTemplate("square", 0, window_p.loadTexture("resources/fair_and_square/square_v2_green.png"));
	_spriteLib.registerAltTextureForTemplate("square", 1, window_p.loadTexture("resources/fair_and_square/square_v2_red.png"));
	_spriteLib.addDyingState("square", 2);
	_spriteLib.registerSpriteTemplate("triangle", window_p.loadTexture("resources/fair_and_square/triangle.png"), 0.5, 16, 16, 32, 32, {8, 8, 8}, {0.25, 0.5/7., 0.05}, 1);
	_spriteLib.registerAltTextureForTemplate("triangle", 0, window_p.loadTexture("resources/fair_and_square/triangle_v2_green.png"));
	_spriteLib.registerAltTextureForTemplate("triangle", 1, window_p.loadTexture("resources/fair_and_square/triangle_v2_red.png"));
	_spriteLib.addDyingState("triangle", 2);
	_spriteLib.registerSpriteTemplate("circle", window_p.loadTexture("resources/fair_and_square/circle.png"), 0.5, 16, 16, 32, 32, {8, 8, 8}, {0.25, 0.5/7., 0.05}, 1);
	_spriteLib.registerAltTextureForTemplate("circle", 0, window_p.loadTexture("resources/fair_and_square/circle_v2_green.png"));
	_spriteLib.registerAltTextureForTemplate("circle", 1, window_p.loadTexture("resources/fair_and_square/circle_v2_red.png"));
	_spriteLib.addDyingState("circle", 2);

	_tilemap.generate();
	_minimap.generate(window_p, _tilemap, fullWorldSize_p);
}

} // namespace fas
