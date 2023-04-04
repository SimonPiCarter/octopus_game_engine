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

#include "core/game/level1/Level1.hh"
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

void runLevel1(Window &window_p, unsigned long stepCount_p, unsigned long worldSize_p, unsigned long seed_p)
{
	octopus::RandomGenerator rand_l(seed_p);
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = fas::level1::WaveLevelSteps(window_p, lib_l, rand_l, 10, stepCount_p, worldSize_p, std::bind(genDialogStep, window_p));
	std::list<octopus::Command *> commands_l = fas::level1::WaveLevelCommands(lib_l, rand_l, worldSize_p);


	std::ofstream file_l("level1.fas", std::ios::out | std::ios::binary);

    file_l.write((char*)&seed_p, sizeof(seed_p));
    file_l.write((char*)&stepCount_p, sizeof(stepCount_p));
    file_l.write((char*)&worldSize_p, sizeof(worldSize_p));

	runAndSaveGame(window_p, spawners_l, commands_l, worldSize_p, file_l, lib_l);
}

void replayLevel1(Window &window_p)
{
	std::ifstream file_l("level1.fas", std::ios::in | std::ios::binary);
	unsigned long seed_p;
	unsigned long stepCount_p;
	unsigned long worldSize_p;

    file_l.read((char*)&seed_p, sizeof(seed_p));
    file_l.read((char*)&stepCount_p, sizeof(stepCount_p));
    file_l.read((char*)&worldSize_p, sizeof(worldSize_p));

	octopus::RandomGenerator rand_l(seed_p);
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = fas::level1::WaveLevelSteps(window_p, lib_l, rand_l, 10, stepCount_p, worldSize_p, std::bind(genDialogStep, window_p));
	std::list<octopus::Command *> commands_l = fas::level1::WaveLevelCommands(lib_l, rand_l, worldSize_p);

	replayGame(file_l, window_p, spawners_l, commands_l, worldSize_p, lib_l);
}

void loadLevel1(Window &window_p)
{
	std::ifstream file_l("level1.fas", std::ios::in | std::ios::binary);
	unsigned long seed_p;
	unsigned long stepCount_p;
	unsigned long worldSize_p;

    file_l.read((char*)&seed_p, sizeof(seed_p));
    file_l.read((char*)&stepCount_p, sizeof(stepCount_p));
    file_l.read((char*)&worldSize_p, sizeof(worldSize_p));

	octopus::RandomGenerator rand_l(seed_p);
	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = fas::level1::WaveLevelSteps(window_p, lib_l, rand_l, 10, stepCount_p, worldSize_p, std::bind(genDialogStep, window_p));
	std::list<octopus::Command *> commands_l = fas::level1::WaveLevelCommands(lib_l, rand_l, worldSize_p);

	loadGame(file_l, window_p, spawners_l, commands_l, worldSize_p, lib_l);
}


} // namespace cuttlefish
