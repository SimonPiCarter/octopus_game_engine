#include "LevelTestModelLoader.h"

#include <iostream>
#include <fstream>
#include <random>

// octopus
#include "controller/trigger/Listener.hh"
#include "command/entity/EntityAttackMoveCommand.hh"
#include "command/spawner/AreaSpawnerCommand.hh"
#include "library/Library.hh"
#include "state/entity/Building.hh"
#include "state/entity/Unit.hh"
#include "state/entity/Resource.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/vision/PatternHandler.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/command/flying/FlyingCommandSpawnStep.hh"
#include "step/custom/CustomStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/state/StateTemplePositionAddStep.hh"
#include "step/state/StateWinStep.hh"
#include "step/team/TeamVisionStep.hh"
#include "step/trigger/TriggerSpawn.hh"

// godot
#include "controller/step/CameraStep.h"
#include "controller/step/DialogStep.h"

using namespace octopus;

namespace godot
{
namespace level_test_model
{

std::list<Steppable *> LevelSteps(Library &lib_p, RandomGenerator &rand_p)
{
	std::list<Steppable *> spawners_l =
	{
		new godot::CameraStep(20, 10),
	};

	return spawners_l;
}

std::list<Command *> LevelCommands(Library &lib_p, RandomGenerator &rand_p)
{
	std::list<Command *> commands_l {
	};

	return commands_l;
}

/// @brief write header for classic arena level
void writeLevelHeader(std::ofstream &file_p, ModelLoaderHeader const &header_p)
{
    file_p.write((char*)&header_p.seed, sizeof(header_p.seed));
}

/// @brief read header for classic arena level and return a pair of steppable and command
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p, ModelLoaderHeader &header_r)
{
    file_p.read((char*)&header_r.seed, sizeof(header_r.seed));

	delete rand_p;
	rand_p = new octopus::RandomGenerator(header_r.seed);

	std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > pair_l;
	pair_l.first = LevelSteps(lib_p, *rand_p);
	pair_l.second = LevelCommands(lib_p, *rand_p);
	return pair_l;
}

} // namespace level_test_model
} // namespace godot
