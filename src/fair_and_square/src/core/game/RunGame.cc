#include "RunGame.hh"

// cuttlefish
#include "window/Window.hh"
#include "world/World.hh"
#include "game/GameLoop.hh"

// octopus
#include "controller/Controller.hh"
#include "library/Library.hh"
#include "logger/Logger.hh"

#include "library/levels/WaveLevel.hh"
#include "library/levels/ArenaLevel.hh"
#include "core/game/loader/ResourceLoader.hh"

using namespace cuttlefish;

namespace fas
{

void runGame(Window &window_p, std::list<octopus::Steppable *> &spawners_p, std::list<octopus::Command *> &commands_p, unsigned long worldSize_p)
{
	unsigned long gridPointSize_l = 5;
	unsigned long gridSize_l = worldSize_p/gridPointSize_l;

	window_p.setWorldSize(worldSize_p);

    Texture const * background_l = window_p.loadTexture("resources/background.png");

	World world_l(0);

	octopus::Controller controller_l(spawners_p, 0.01, commands_p, gridPointSize_l, gridSize_l);
	controller_l.enableORCA();

	fas::RessourceLoader loader_l(window_p, worldSize_p, world_l);

	GameLoop loop_l(
		loader_l._descPanel,
		loader_l._choicePanel,
		loader_l._minimap,
		controller_l,
		loader_l._panel,
		loader_l._spriteLib,
		loader_l._statsPanel,
		loader_l._tilemap,
		world_l);

	loop_l.runLoop(window_p);
}

void runWave(Window &window_p, unsigned long stepCount_p, unsigned long player_p, unsigned long worldSize_p)
{
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = WaveLevelSteps(lib_l, 10, stepCount_p, player_p, worldSize_p);
	std::list<octopus::Command *> commands_l = WaveLevelCommands(lib_l, worldSize_p);

	runGame(window_p, spawners_l, commands_l, worldSize_p);
}

void runArena(cuttlefish::Window &window_p, size_t number_p)
{
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = ArenaLevelSteps(lib_l, number_p);
	std::list<octopus::Command *> commands_l = ArenaLevelCommands(lib_l);

	runGame(window_p, spawners_l, commands_l, 250);
}


} // namespace cuttlefish
