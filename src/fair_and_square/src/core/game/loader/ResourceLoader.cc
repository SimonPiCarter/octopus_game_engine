#include "ResourceLoader.hh"

#include "window/Window.hh"
#include "world/World.hh"

namespace fas
{
RessourceLoader::RessourceLoader(cuttlefish::Window &window_p, unsigned long fullWorldSize_p, cuttlefish::World &world_p)
	: _panel(&window_p, window_p.getWidth()-261, window_p.getHeight()-261,
		window_p.loadTexture("resources/background.png"), window_p.loadTexture("resources/grid.png"), 4)
	, _statsPanel(&window_p, window_p.getWidth()-522, window_p.getHeight()-261,
		window_p.loadTexture("resources/background.png"), window_p.loadTexture("resources/grid.png"),
		window_p.loadTexture("resources/fair_and_square/prod_bar_back.png"), window_p.loadTexture("resources/fair_and_square/prod_bar_fill.png"), 4, world_p.getSelection())
	, _divPanel(&window_p, window_p.getWidth()-window_p.getWidth()/2-200, window_p.getHeight()-window_p.getHeight()/2-200,
		window_p.loadTexture("resources/background.png"), window_p.loadTexture("resources/grid.png"), 0)
	, _descPanel(&window_p, window_p.getWidth()-260, window_p.getHeight()-520,
		window_p.loadTexture("resources/background.png"))
	, _spriteLib()
	, _tilemap(fullWorldSize_p, _spriteLib, "tiles", "details")
	, _minimap(window_p, 0, window_p.getHeight()-250, 250, 250, {"resources/me.png", "resources/enemy.png", "resources/ally.png"})
{
	_panel.addSpriteInfo("worker", 2, 1);
	_panel.addSpriteInfo("command_center", 1, 0);
	_panel.addSpriteInfo("barrack", 0, 0);

	_panel.addSpriteInfo("square", 1, 4);
	_panel.addSpriteInfo("triangle", 2, 3);
	_panel.addSpriteInfo("circle", 1, 3);

	// resources
	_statsPanel.addSpriteInfo("resource_steel", 2, 4);
	_statsPanel.addSpriteInfo("resource_food", 2, 4);

	_statsPanel.addSpriteInfo("worker", 2, 1);
	_statsPanel.addSpriteInfo("command_center", 1, 0);
	_statsPanel.addSpriteInfo("barrack", 0, 0);

	_statsPanel.addSpriteInfo("square", 1, 4);
	_statsPanel.addSpriteInfo("triangle", 2, 3);
	_statsPanel.addSpriteInfo("circle", 1, 3);

	_divPanel.addOptionInfo(octopus::DivinityType::Divinity_1, 2, 0);
	_divPanel.addOptionInfo(octopus::DivinityType::Divinity_3, 1, 3);
	_divPanel.addOptionInfo(octopus::DivinityType::Divinity_4, 1, 4);
	_divPanel.addOptionInfo(octopus::DivinityType::Divinity_5, 2, 3);

	_descPanel.setText("ceci est un texte de test avec un parametre param $param");

	// hp bar
	_spriteLib.setHpBarBackground(window_p.loadTexture("resources/fair_and_square/hp_bar_back.png"));
	_spriteLib.setHpBarFilling(window_p.loadTexture("resources/fair_and_square/hp_bar_fill.png"));
	// tile map
	_spriteLib.registerSpriteTemplate("tiles", window_p.loadTexture("resources/fair_and_square/tiles.png"), 1., 0, 0, 64, 64, std::vector<int>(16, 1), std::vector<double>(16, 1), 1);
	_spriteLib.registerSpriteTemplate("details", window_p.loadTexture("resources/fair_and_square/details.png"), 0.5, 0, 0, 64, 64, {1}, {2}, 1);

	// resources
	_spriteLib.registerSpriteTemplate("resource_steel", window_p.loadTexture("resources/steel_prop1.png"), 2., 128, 128, 256, 256, {10, 10}, {0.2,0.2}, 1);
	_spriteLib.registerSpriteTemplate("resource_food", window_p.loadTexture("resources/fair_and_square/food_prop2.png"), 2., 64, 64, 128, 128, {2, 2}, {0.5, 1.}, 1);

	// neutral
	_spriteLib.registerSpriteTemplate("command_center", window_p.loadTexture("resources/fair_and_square/command_center.png"), 2., 64, 64, 128, 128, {8, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);
	_spriteLib.registerSpriteTemplate("barrack", window_p.loadTexture("resources/deposit.png"), 1., 32, 32, 64, 64, {2, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);
	_spriteLib.registerSpriteTemplate("worker", window_p.loadTexture("resources/worker.png"), 0.5, 32, 32, 64, 64, {2, 2}, {0.25, 1}, 1);

	_spriteLib.registerSpriteTemplate("square", window_p.loadTexture("resources/fair_and_square/square.png"), 0.5, 16, 16, 32, 32, {8, 8}, {0.25, 0.5/7.}, 1);
	_spriteLib.registerSpriteTemplate("triangle", window_p.loadTexture("resources/fair_and_square/triangle.png"), 0.5, 16, 16, 32, 32, {8, 8}, {0.25, 0.5/7.}, 1);
	_spriteLib.registerSpriteTemplate("circle", window_p.loadTexture("resources/fair_and_square/circle.png"), 0.5, 16, 16, 32, 32, {8, 8}, {0.25, 0.5/7.}, 1);

	_tilemap.generate();
	_minimap.generate(window_p, _tilemap, fullWorldSize_p);
}

} // namespace fas
