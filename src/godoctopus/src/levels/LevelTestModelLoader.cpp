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
	Building building_l({20, 10}, true, lib_p.getBuildingModel("barrack_circle"));
	Unit unit_l({ 17, 10 }, false, lib_p.getUnitModel("worker"));

	long model_l = 420;
	std::map<std::string, Fixed> mapRes_l;
	mapRes_l["Food"] = -2000;
	mapRes_l["Steel"] = -2000;

	Handle handle_l = 0;
	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("barrack_circle")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("deposit")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("anchor")),
		new PlayerSpendResourceStep(0, mapRes_l),
		new BuildingSpawnStep(handle_l++, building_l, true),
		new UnitSpawnStep(handle_l++, unit_l),
		new godot::CameraStep(to_int(building_l._pos.x), to_int(building_l._pos.y)),
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
void writeLevelHeader(std::ofstream &file_p, int seed_p)
{
    file_p.write((char*)&seed_p, sizeof(seed_p));
}

/// @brief read header for classic arena level and return a pair of steppable and command
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p)
{
	int seed_l;
    file_p.read((char*)&seed_l, sizeof(seed_l));

	delete rand_p;
	rand_p = new octopus::RandomGenerator(seed_l);

	std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > pair_l;
	pair_l.first = LevelSteps(lib_p, *rand_p);
	pair_l.second = LevelCommands(lib_p, *rand_p);
	return pair_l;
}

} // namespace level_test_model
} // namespace godot
