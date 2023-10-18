#include "LevelTestModelLoader.h"

#include <iostream>
#include <fstream>
#include <random>

// fas
#include "library/model/AnchorDivinityTrigger.hh"
#include "library/model/divinity/DivinityModelLoader.hh"
#include "library/model/divinity/generators/UnitModelIds.hh"
#include "library/model/ModelLoader.hh"

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
#include "step/player/PlayerLevelUpUpgradeStep.hh"
#include "step/player/PlayerBuffAllStep.hh"
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

std::list<Steppable *> LevelSteps(Library &lib_p, RandomGenerator &rand_p, bool buffProd_p)
{
	loadMinimalModels(lib_p);

	// load divinity library
	fas::loadLibrary(lib_p);

	std::list<Steppable *> spawners_l =
	{
		new godot::CameraStep(20, 10, 0),
		new PlayerLevelUpUpgradeStep(0, models::AttackSpeedDivId),
		new PlayerLevelUpUpgradeStep(0, models::AttackSpeedDivId),
		new PlayerLevelUpUpgradeStep(0, models::AttackSpeedDivId),
		new PlayerLevelUpUpgradeStep(0, models::EconomicDivId),
		new PlayerLevelUpUpgradeStep(0, models::EconomicDivId),
		new PlayerLevelUpUpgradeStep(0, models::EconomicDivId),
		new PlayerLevelUpUpgradeStep(0, models::HealDivId),
		new PlayerLevelUpUpgradeStep(0, models::HealDivId),
		new PlayerLevelUpUpgradeStep(0, models::HealDivId),
		new PlayerLevelUpUpgradeStep(0, models::LifestealDivId),
		new PlayerLevelUpUpgradeStep(0, models::LifestealDivId),
		new PlayerLevelUpUpgradeStep(0, models::LifestealDivId),
		new PlayerLevelUpUpgradeStep(0, models::ProductionDivId),
		new PlayerLevelUpUpgradeStep(0, models::ProductionDivId),
		new PlayerLevelUpUpgradeStep(0, models::ProductionDivId),
		new PlayerLevelUpUpgradeStep(0, models::RecycleDivId),
		new PlayerLevelUpUpgradeStep(0, models::RecycleDivId),
		new PlayerLevelUpUpgradeStep(0, models::RecycleDivId),
		new TriggerSpawn(new AnchorDivinityTrigger(lib_p, rand_p, 0, {
			fas::DivinityType::AttackSpeed,
			fas::DivinityType::Economic,
			fas::DivinityType::Heal,
			fas::DivinityType::Lifesteal,
			fas::DivinityType::Production,
			fas::DivinityType::Recycle,
		}, 180)),
	};

	if(buffProd_p)
	{
		octopus::TimedBuff buff_l;
		buff_l._id = "model_loading_buff_prod";
		buff_l._type = octopus::TyppedBuff::Type::Production;
		buff_l._coef = 9.;
		spawners_l.push_back(new octopus::PlayerBuffAllStep(0, buff_l, ""));
	}

	fas::addBuildingPlayer(spawners_l, 2, fas::allDivinities(), lib_p);

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
    file_p.write((char*)&header_p.buff_prod, sizeof(header_p.buff_prod));
}

/// @brief read header for classic arena level and return a pair of steppable and command
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p, ModelLoaderHeader &header_r)
{
    file_p.read((char*)&header_r.seed, sizeof(header_r.seed));
    file_p.read((char*)&header_r.buff_prod, sizeof(header_r.buff_prod));

	delete rand_p;
	rand_p = new octopus::RandomGenerator(header_r.seed);

	std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > pair_l;
	pair_l.first = LevelSteps(lib_p, *rand_p, header_r.buff_prod);
	pair_l.second = LevelCommands(lib_p, *rand_p);
	return pair_l;
}

} // namespace level_test_model
} // namespace godot
