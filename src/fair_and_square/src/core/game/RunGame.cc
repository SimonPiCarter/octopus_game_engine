#include "RunGame.hh"

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

#include "RunnerHelpers.hh"

#include <fstream>

using namespace cuttlefish;

namespace fas
{

void runWave(Window &window_p, unsigned long stepCount_p, unsigned long player_p, unsigned long worldSize_p, unsigned long seed_p)
{
	octopus::RandomGenerator rand_l(seed_p);
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = WaveLevelSteps(lib_l, rand_l, 10, stepCount_p, player_p, worldSize_p, std::bind(genDialogStep, window_p));
	std::list<octopus::Command *> commands_l = WaveLevelCommands(lib_l, rand_l, worldSize_p);


	std::ofstream file_l("wave.fas", std::ios::out | std::ios::binary);

    file_l.write((char*)&seed_p, sizeof(seed_p));
    file_l.write((char*)&stepCount_p, sizeof(stepCount_p));
    file_l.write((char*)&player_p, sizeof(player_p));
    file_l.write((char*)&worldSize_p, sizeof(worldSize_p));

	runAndSaveGame(window_p, spawners_l, commands_l, worldSize_p, file_l, lib_l);
}

void replayWave(Window &window_p)
{
	std::ifstream file_l("wave.fas", std::ios::in | std::ios::binary);
	unsigned long seed_p;
	unsigned long stepCount_p;
	unsigned long player_p;
	unsigned long worldSize_p;

    file_l.read((char*)&seed_p, sizeof(seed_p));
    file_l.read((char*)&stepCount_p, sizeof(stepCount_p));
    file_l.read((char*)&player_p, sizeof(player_p));
    file_l.read((char*)&worldSize_p, sizeof(worldSize_p));

	octopus::RandomGenerator rand_l(seed_p);
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = WaveLevelSteps(lib_l, rand_l, 10, stepCount_p, player_p, worldSize_p, std::bind(genDialogStep, window_p));
	std::list<octopus::Command *> commands_l = WaveLevelCommands(lib_l, rand_l, worldSize_p);

	replayGame(file_l, window_p, spawners_l, commands_l, worldSize_p, lib_l);
}

void loadWave(Window &window_p)
{
	std::ifstream file_l("wave.fas", std::ios::in | std::ios::binary);
	unsigned long seed_p;
	unsigned long stepCount_p;
	unsigned long player_p;
	unsigned long worldSize_p;

    file_l.read((char*)&seed_p, sizeof(seed_p));
    file_l.read((char*)&stepCount_p, sizeof(stepCount_p));
    file_l.read((char*)&player_p, sizeof(player_p));
    file_l.read((char*)&worldSize_p, sizeof(worldSize_p));

	octopus::RandomGenerator rand_l(seed_p);
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = WaveLevelSteps(lib_l, rand_l, 10, stepCount_p, player_p, worldSize_p, std::bind(genDialogStep, window_p));
	std::list<octopus::Command *> commands_l = WaveLevelCommands(lib_l, rand_l, worldSize_p);

	loadGame(file_l, window_p, spawners_l, commands_l, worldSize_p, lib_l);
}

void runArena(cuttlefish::Window &window_p, unsigned long number_p)
{
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = ArenaLevelSteps(lib_l, number_p);
	std::list<octopus::Command *> commands_l = ArenaLevelCommands(lib_l);

	std::ofstream file_l("arena.fas", std::ios::out | std::ios::binary);

    file_l.write((char*)&number_p, sizeof(number_p));

	runAndSaveGame(window_p, spawners_l, commands_l, 250, file_l, lib_l);
}

void replayArena(Window &window_p)
{
	std::ifstream file_l("arena.fas", std::ios::in | std::ios::binary);
	unsigned long number_p;

    file_l.read((char*)&number_p, sizeof(number_p));

	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = ArenaLevelSteps(lib_l, number_p);
	std::list<octopus::Command *> commands_l = ArenaLevelCommands(lib_l);

	replayGame(file_l, window_p, spawners_l, commands_l, 250, lib_l);
}

void loadArena(Window &window_p)
{
	std::ifstream file_l("arena.fas", std::ios::in | std::ios::binary);
	unsigned long number_p;

    file_l.read((char*)&number_p, sizeof(number_p));

	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = ArenaLevelSteps(lib_l, number_p);
	std::list<octopus::Command *> commands_l = ArenaLevelCommands(lib_l);

	loadGame(file_l, window_p, spawners_l, commands_l, 250, lib_l);
}

void runMaze(cuttlefish::Window &window_p, unsigned long number_p)
{
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = MazeLevelSteps(lib_l, number_p);
	std::list<octopus::Command *> commands_l = MazeLevelCommands(lib_l);

	runGame(window_p, spawners_l, commands_l, 250, lib_l);
}


} // namespace cuttlefish
