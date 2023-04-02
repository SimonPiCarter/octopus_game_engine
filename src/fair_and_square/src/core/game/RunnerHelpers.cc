#include "RunnerHelpers.hh"

// cuttlefish
#include "window/Window.hh"
#include "world/World.hh"
#include "game/GameLoop.hh"

// octopus
#include "controller/Controller.hh"
#include "library/Library.hh"
#include "logger/Logger.hh"
#include "state/State.hh"
#include "step/trigger/TriggerSpawn.hh"

#include "library/levels/WaveLevel.hh"
#include "library/levels/ArenaLevel.hh"
#include "library/levels/MazeLevel.hh"
#include "core/game/loader/ResourceLoader.hh"
#include "core/game/loader/trigger/AnchorEffectTrigger.hh"
#include "core/game/loader/visitor/FASStepVisitor.hh"
#include "core/lang/LangEntries.hh"

#include "serialization/CommandSerialization.hh"



using namespace cuttlefish;

namespace fas
{

/// @brief Run the game
void runGame(Window &window_p, std::list<octopus::Steppable *> &spawners_p, std::list<octopus::Command *> &commands_p, unsigned long worldSize_p, octopus::Library &lib_p)
{
	unsigned long gridPointSize_l = 5;
	unsigned long gridSize_l = worldSize_p/gridPointSize_l;

	window_p.setWorldSize(worldSize_p);

	World world_l(0);

	FASStepVisitor visitor_l(window_p, world_l);
	spawners_p.push_back(new octopus::TriggerSpawn(new AnchorEffectTrigger(lib_p, visitor_l)));
	world_l.setCustomVisitor(&visitor_l);

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

/// @brief Run a game and save it a the end
void runAndSaveGame(Window &window_p, std::list<octopus::Steppable *> &spawners_p, std::list<octopus::Command *> &commands_p, unsigned long worldSize_p, std::ofstream &file_p, octopus::Library &lib_p)
{
	unsigned long gridPointSize_l = 5;
	unsigned long gridSize_l = worldSize_p/gridPointSize_l;

	window_p.setWorldSize(worldSize_p);

	World world_l(0);

	FASStepVisitor visitor_l(window_p, world_l);
	spawners_p.push_back(new octopus::TriggerSpawn(new AnchorEffectTrigger(lib_p, visitor_l)));
	world_l.setCustomVisitor(&visitor_l);

	octopus::Controller controller_l(spawners_p, 0.01, commands_p, gridPointSize_l, gridSize_l);
	controller_l.enableORCA();

	// neeed nb of steps in header of the file
	size_t header_l = 0;
	file_p.write((char*)&header_l, sizeof(header_l));
	controller_l.setOnlineSaveFile(&file_p);

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

/// @brief Replay the game to watch it and stop at end of save
void replayGame(std::ifstream &file_p, Window &window_p, std::list<octopus::Steppable *> &spawners_p, std::list<octopus::Command *> &commands_p, unsigned long worldSize_p, octopus::Library const &lib_p)
{
	unsigned long gridPointSize_l = 5;
	unsigned long gridSize_l = worldSize_p/gridPointSize_l;

	window_p.setWorldSize(worldSize_p);

	World world_l(0);

	FASStepVisitor visitor_l(window_p, world_l);
	spawners_p.push_back(new octopus::TriggerSpawn(new AnchorEffectTrigger(lib_p, visitor_l)));
	world_l.setCustomVisitor(&visitor_l);

	octopus::Controller controller_l(spawners_p, 0.01, commands_p, gridPointSize_l, gridSize_l);
	controller_l.enableORCA();

	fas::ResourceLoader loader_l(window_p, worldSize_p, world_l);

	readCommands(file_p, controller_l, lib_p);

	controller_l.setOngoingStep(1);
	controller_l.setReplayMode(true);

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

/// @brief Load game and start from last step saved
void loadGame(std::ifstream &file_p, Window &window_p, std::list<octopus::Steppable *> &spawners_p, std::list<octopus::Command *> &commands_p, unsigned long worldSize_p, octopus::Library const &lib_p)
{
	unsigned long gridPointSize_l = 5;
	unsigned long gridSize_l = worldSize_p/gridPointSize_l;

	window_p.setWorldSize(worldSize_p);

	World world_l(0);

	FASStepVisitor visitor_l(window_p, world_l);
	spawners_p.push_back(new octopus::TriggerSpawn(new AnchorEffectTrigger(lib_p, visitor_l)));
	world_l.setCustomVisitor(&visitor_l);

	octopus::Controller controller_l(spawners_p, 0.01, commands_p, gridPointSize_l, gridSize_l);
	controller_l.enableORCA();

	fas::ResourceLoader loader_l(window_p, worldSize_p, world_l);

	readCommands(file_p, controller_l, lib_p);

	auto last_l = std::chrono::steady_clock::now();

	std::cout<<"loading..."<<std::endl;
	while(!controller_l.loop_body())
	{
		octopus::StateAndSteps stateAndSteps_l = controller_l.queryStateAndSteps();
		octopus::State const &state_l = *stateAndSteps_l._state;
		world_l.handleStep(window_p, loader_l._panel, loader_l._statsPanel, loader_l._choicePanel, stateAndSteps_l, loader_l._spriteLib);
	}

	auto cur_l = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds_l = cur_l-last_l;
	double elapsed_l = elapsed_seconds_l.count();

	std::cout<<"done in "<<(int)(elapsed_l*1000)<<"ms"<<std::endl;

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


std::vector<octopus::Steppable*> genDialogStep(Window &window_p)
{
	return {
		new cuttlefish::DialogStep(LangEntries::GetInstance()->getEntry("wave_incoming_name"), LangEntries::GetInstance()->getEntry("wave_incoming_first"),
			cuttlefish::Picture(window_p.loadTexture("resources/octopus.png"), 64, 64, {2}, {1})),
		new cuttlefish::DialogStep(LangEntries::GetInstance()->getEntry("wave_incoming_name"), LangEntries::GetInstance()->getEntry("wave_incoming_second"),
			cuttlefish::Picture(window_p.loadTexture("resources/octopus.png"), 64, 64, {2}, {1}))
	};
}

} // namespace fas
