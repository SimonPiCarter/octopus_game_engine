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
#include "file/FileHeader.h"
#include "library/FirstRunicBoss.h"
#include "library/model/survival/SurvivalModelLoader.hh"

using namespace octopus;

namespace godot
{
namespace level_test_model
{

std::list<Steppable *> LevelSteps(Library &lib_p, RandomGenerator &rand_p, bool buffProd_p, bool upgrades_rune_p,
	unsigned long idx_first_player_p, unsigned long nb_players_p)
{
	loadMinimalModels(lib_p);
	loadSurvivalModels(lib_p);

	// load divinity library
	fas::loadLibrary(lib_p);
	addFirstRunicBossToLibrary(lib_p);

	std::list<Steppable *> spawners_l = {};

	if(buffProd_p)
	{
		octopus::TimedBuff buff_l;
		buff_l._id = "model_loading_buff_prod";
		buff_l._type = octopus::TyppedBuff::Type::Production;
		buff_l._coef = 9.;
		for(unsigned long player_l = 0 ; player_l < nb_players_p+idx_first_player_p ; ++player_l )
		{
			spawners_l.push_back(new octopus::PlayerBuffAllStep(player_l, buff_l, ""));
		}
	}

	if(upgrades_rune_p)
	{
		for(unsigned long player_l = 0 ; player_l < nb_players_p+idx_first_player_p ; ++player_l )
		{
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::AttackSpeedDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::AttackSpeedDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::AttackSpeedDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::EconomicDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::EconomicDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::EconomicDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::HealDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::HealDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::HealDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::LifestealDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::LifestealDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::LifestealDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::ProductionDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::ProductionDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::ProductionDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::RecycleDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::RecycleDivId));
			spawners_l.push_back(new PlayerLevelUpUpgradeStep(player_l, models::RecycleDivId));
		}
	}

	for(unsigned long player_l = 0 ; player_l < nb_players_p+idx_first_player_p ; ++player_l )
	{
		spawners_l.push_back(
			new TriggerSpawn(new AnchorDivinityTrigger(lib_p, rand_p, player_l, fas::demoDivinities(), 180))
		);
		fas::addDivinityBuildingPlayer(spawners_l, player_l, fas::demoDivinities(), lib_p);
	}

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
    file_p.write((char*)&header_p.upgrade_rune, sizeof(header_p.upgrade_rune));
}

/// @brief read header for classic arena level and return a pair of steppable and command
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p, ModelLoaderHeader &header_r, FileHeader const &_fileHeader)
{
    file_p.read((char*)&header_r.seed, sizeof(header_r.seed));
    file_p.read((char*)&header_r.buff_prod, sizeof(header_r.buff_prod));
    file_p.read((char*)&header_r.upgrade_rune, sizeof(header_r.upgrade_rune));

	delete rand_p;
	rand_p = new octopus::RandomGenerator(header_r.seed);

	std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > pair_l;
	pair_l.first = LevelSteps(lib_p, *rand_p, header_r.buff_prod, header_r.upgrade_rune, _fileHeader.get_idx_first_player(), _fileHeader.get_num_players());
	pair_l.second = LevelCommands(lib_p, *rand_p);
	return pair_l;
}

} // namespace level_test_model
} // namespace godot
