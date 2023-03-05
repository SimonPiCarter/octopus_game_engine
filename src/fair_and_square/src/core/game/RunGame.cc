#include "RunGame.hh"

// cuttlefish
#include "minimap/Minimap.hh"
#include "panel/DescPanel.hh"
#include "panel/DivinityPanel.hh"
#include "panel/Panel.hh"
#include "panel/StatsPanel.hh"
#include "sprite/SpriteLibrary.hh"
#include "texture/Texture.hh"
#include "tilemap/Tilemap.hh"
#include "window/Window.hh"
#include "world/World.hh"
#include "game/GameLoop.hh"

// octopus
#include "controller/Controller.hh"
#include "library/Library.hh"
#include "logger/Logger.hh"

#include "library/levels/WaveLevel.hh"

namespace cuttlefish
{

void runGame(Window &window_p)
{
	unsigned long gridSize_l = 50;
	unsigned long gridPointSize_l = 5;
	unsigned long fullWorldSize_l = gridPointSize_l * gridSize_l;

	window_p.setWorldSize(fullWorldSize_l);

    Texture const * background_l = window_p.loadTexture("resources/background.png");

	World world_l;

	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = WaveLevelSteps(lib_l, 10, 3*60*100);
	std::list<octopus::Command *> commands_l = WaveLevelCommands(lib_l);

	octopus::Controller controller_l(spawners_l, 0.01, commands_l, gridPointSize_l, gridSize_l);

	Panel panel_l(&window_p, window_p.getWidth()-261, window_p.getHeight()-261,
		window_p.loadTexture("resources/background.png"), window_p.loadTexture("resources/grid.png"), 4);

	panel_l.addSpriteInfo("worker", 2, 1);
	panel_l.addSpriteInfo("command_center", 1, 0);
	panel_l.addSpriteInfo("barrack", 0, 0);

	panel_l.addSpriteInfo("square", 1, 4);
	panel_l.addSpriteInfo("triangle", 2, 3);
	panel_l.addSpriteInfo("circle", 1, 3);

	StatsPanel statsPanel_l(&window_p, window_p.getWidth()-522, window_p.getHeight()-261,
		window_p.loadTexture("resources/background.png"), window_p.loadTexture("resources/grid.png"),
		window_p.loadTexture("resources/fair_and_square/prod_bar_back.png"), window_p.loadTexture("resources/fair_and_square/prod_bar_fill.png"), 4, world_l.getSelection());

	// resources
	statsPanel_l.addSpriteInfo("resource_steel", 2, 4);
	statsPanel_l.addSpriteInfo("resource_food", 2, 4);

	statsPanel_l.addSpriteInfo("worker", 2, 1);
	statsPanel_l.addSpriteInfo("command_center", 1, 0);
	statsPanel_l.addSpriteInfo("barrack", 0, 0);

	statsPanel_l.addSpriteInfo("square", 1, 4);
	statsPanel_l.addSpriteInfo("triangle", 2, 3);
	statsPanel_l.addSpriteInfo("circle", 1, 3);

	DivinityPanel divPanel_l(&window_p, window_p.getWidth()-window_p.getWidth()/2-200, window_p.getHeight()-window_p.getHeight()/2-200,
		window_p.loadTexture("resources/background.png"), window_p.loadTexture("resources/grid.png"), 0);
	divPanel_l.addOptionInfo(octopus::DivinityType::Divinity_1, 2, 0);
	divPanel_l.addOptionInfo(octopus::DivinityType::Divinity_3, 1, 3);
	divPanel_l.addOptionInfo(octopus::DivinityType::Divinity_4, 1, 4);
	divPanel_l.addOptionInfo(octopus::DivinityType::Divinity_5, 2, 3);

	DescPanel descPanel_l(&window_p, window_p.getWidth()-260, window_p.getHeight()-520,
		window_p.loadTexture("resources/background.png"));
	descPanel_l.setText("ceci est un texte de test avec un parametre param $param");

	SpriteLibrary spriteLib_l;
	// hp bar
	spriteLib_l.setHpBarBackground(window_p.loadTexture("resources/fair_and_square/hp_bar_back.png"));
	spriteLib_l.setHpBarFilling(window_p.loadTexture("resources/fair_and_square/hp_bar_fill.png"));
	// tile map
	spriteLib_l.registerSpriteTemplate("tiles", window_p.loadTexture("resources/fair_and_square/tiles.png"), 1., 0, 0, 64, 64, std::vector<int>(16, 1), std::vector<double>(16, 1), 1);
	spriteLib_l.registerSpriteTemplate("details", window_p.loadTexture("resources/fair_and_square/details.png"), 0.5, 0, 0, 64, 64, {1}, {2}, 1);

	// resources
	spriteLib_l.registerSpriteTemplate("resource_steel", window_p.loadTexture("resources/steel_prop1.png"), 2., 128, 128, 256, 256, {10, 10}, {0.2,0.2}, 1);
	spriteLib_l.registerSpriteTemplate("resource_food", window_p.loadTexture("resources/fair_and_square/food_prop2.png"), 2., 64, 64, 128, 128, {2, 2}, {0.5, 1.}, 1);

	// neutral
	spriteLib_l.registerSpriteTemplate("command_center", window_p.loadTexture("resources/fair_and_square/command_center.png"), 2., 64, 64, 128, 128, {8, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);
	spriteLib_l.registerSpriteTemplate("barrack", window_p.loadTexture("resources/deposit.png"), 1., 32, 32, 64, 64, {2, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);
	spriteLib_l.registerSpriteTemplate("worker", window_p.loadTexture("resources/worker.png"), 0.5, 32, 32, 64, 64, {2, 2}, {0.25, 1}, 1);

	spriteLib_l.registerSpriteTemplate("square", window_p.loadTexture("resources/fair_and_square/square.png"), 0.5, 16, 16, 32, 32, {8, 8}, {0.25, 0.5/7.}, 1);
	spriteLib_l.registerSpriteTemplate("triangle", window_p.loadTexture("resources/fair_and_square/triangle.png"), 0.5, 16, 16, 32, 32, {8, 8}, {0.25, 0.5/7.}, 1);
	spriteLib_l.registerSpriteTemplate("circle", window_p.loadTexture("resources/fair_and_square/circle.png"), 0.5, 16, 16, 32, 32, {8, 8}, {0.25, 0.5/7.}, 1);

	Tilemap tilemap_l(fullWorldSize_l, spriteLib_l, "tiles", "details");
	tilemap_l.generate();

	Minimap minimap_l(window_p, 0, window_p.getHeight()-250, 250, 250, tilemap_l, fullWorldSize_l, {"resources/me.png", "resources/enemy.png", "resources/ally.png"});

	GameLoop loop_l(descPanel_l,
		divPanel_l,
		minimap_l,
		controller_l,
		panel_l,
		spriteLib_l,
		statsPanel_l,
		tilemap_l,
		world_l);

	loop_l.runLoop(window_p);
}

} // namespace cuttlefish
