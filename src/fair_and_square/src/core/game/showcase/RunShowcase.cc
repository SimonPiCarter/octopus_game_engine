#include "RunShowcase.hh"

// cuttlefish
#include "window/Window.hh"
#include "world/World.hh"
#include "game/GameLoop.hh"

// octopus
#include "controller/Controller.hh"
#include "state/State.hh"
#include "library/Library.hh"
#include "logger/Logger.hh"

#include "library/levels/showcase/AoeShowcase.hh"
#include "library/levels/showcase/ChainingShowcase.hh"
#include "library/levels/showcase/DotShowcase.hh"
#include "library/levels/showcase/LifestealShowcase.hh"
#include "core/game/loader/ResourceLoader.hh"

#include <fstream>

using namespace cuttlefish;

namespace fas
{

/// @brief Run the showcase
void runShowcase(Window &window_p, std::list<octopus::Steppable *> &spawners_p, std::list<octopus::Command *> &commands_p, unsigned long worldSize_p)
{
	unsigned long gridPointSize_l = 5;
	unsigned long gridSize_l = worldSize_p/gridPointSize_l;

	window_p.setWorldSize(worldSize_p);

	World world_l(0);

	octopus::Controller controller_l(spawners_p, 0.01, commands_p, gridPointSize_l, gridSize_l);
	controller_l.enableORCA();

	fas::ResourceLoader loader_l(window_p, worldSize_p, world_l);

	GameLoop loop_l(
		loader_l._descPanel,
		loader_l._choicePanel,
		loader_l._minimap,
		controller_l,
		loader_l._panel,
		loader_l._prodPanel,
		loader_l._resourcesPanel,
		loader_l._spriteLib,
		loader_l._statsPanel,
		loader_l._tilemap,
		world_l);

	loop_l.runLoop(window_p);
}

void runAoeShowcase(cuttlefish::Window &window_p)
{
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = aoeShowcaseSteps(lib_l, 500);
	std::list<octopus::Command *> commands_l = aoeShowcaseCommands(lib_l);

    spawners_l.push_back(
		new cuttlefish::DialogStep("Welcome to the AoE Modifier Showcase", "Triangles can't harm you here. $$ Your square has been empowered with an AoE modifier doing 50% damage in a 5tiles radius area. $$ Press any key to start...",
			cuttlefish::Picture(window_p.loadTexture("resources/octopus.png"), 64, 64, {2}, {1}))
    );

	runShowcase(window_p, spawners_l, commands_l, 250);
}

void runChainingShowcase(cuttlefish::Window &window_p)
{
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = chainingShowcaseSteps(lib_l);
	std::list<octopus::Command *> commands_l = chainingShowcaseCommands(lib_l);

    spawners_l.push_back(
		new cuttlefish::DialogStep("Welcome to the Chaining Modifier Showcase", " Triangles can't harm you here. $$ Your square has been empowered with a chaining modifier bouncing to 5 targets with 80% damage decay. $$ Press any key to start...",
			cuttlefish::Picture(window_p.loadTexture("resources/octopus.png"), 64, 64, {2}, {1}))
    );

	runShowcase(window_p, spawners_l, commands_l, 250);
}

void runDotShowcase(cuttlefish::Window &window_p)
{
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = dotShowcaseSteps(lib_l, 50);
	std::list<octopus::Command *> commands_l = dotShowcaseCommands(lib_l);

    spawners_l.push_back(
		new cuttlefish::DialogStep("Welcome to the Dot Modifier Showcase", "Triangles can't harm you here. $$ Your square has been empowered with a dot modifier dealing 10 damage every half second for a total of 50 damage. $$ Press any key to start...",
			cuttlefish::Picture(window_p.loadTexture("resources/octopus.png"), 64, 64, {2}, {1}))
    );

	runShowcase(window_p, spawners_l, commands_l, 250);
}

void runLifestealShowcase(cuttlefish::Window &window_p)
{
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = lifestealShowcaseSteps(lib_l, 1);
	std::list<octopus::Command *> commands_l = lifestealShowcaseCommands(lib_l);

    spawners_l.push_back(
		new cuttlefish::DialogStep("Welcome to the Lifesteal Modifier Showcase", "Your square has been empowered with a lifesteal modifier healing him for 80% of the damage dealt. $$ Triangles WILL harm you here. $$ Press any key to start...",
			cuttlefish::Picture(window_p.loadTexture("resources/octopus.png"), 64, 64, {2}, {1}))
    );

	runShowcase(window_p, spawners_l, commands_l, 250);
}

} // namespace cuttlefish
