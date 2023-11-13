#include "DemoLevel.h"

#include <iostream>
#include <fstream>
#include <random>

// fas
#include "library/utils/Randomizer.hh"
#include "library/utils/LoseTrigger.hh"
#include "library/model/AnchorTrigger.hh"
#include "library/model/ModelLoader.hh"
#include "library/model/TimerDamage.hh"
#include "library/model/divinity/DivinityModelLoader.hh"

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
#include "step/player/PlayerAddBuildingModel.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/state/StateTemplePositionAddStep.hh"
#include "step/team/TeamVisionStep.hh"
#include "step/trigger/TriggerSpawn.hh"
#include "utils/Binary.hh"

// godot
#include "controller/step/CameraStep.h"
#include "controller/step/DialogStep.h"

#include "levels/wave/WaveSpawn.h"
#include "levels/model/utils/EntitySpawner.h"

using namespace octopus;
using namespace fas;

namespace godot
{
namespace demo
{

std::list<Steppable *> DemoLevelSteps(
	Library &lib_p,
	RandomGenerator &rand_p,
	std::vector<WavePoolInfo> const &waveInfo_p,
	unsigned long player_p,
	unsigned long playerCount_p,
	std::vector<GodotEntityInfo> const &entityInfo_p,
	int difficulty_p)
{
	// Tick rate for anchor decay : easy => 1 every 2sec
	// >= medium : 1 every sec
	size_t anchorTickRate_l = difficulty_p < 2 ? 200 : 100;

	// number of spawn points
	unsigned long nSpawnPoints_l = difficulty_p < 2 ? 1 : 2;

	loadMinimalModels(lib_p);

	// load divinity library
	fas::loadLibrary(lib_p);

	std::list<Steppable *> spawners_l = {};

	std::vector<WavePoolInfo> waves_l = waveInfo_p;
	if(waves_l.empty())
	{
		WavePoolInfo pool_l;

		WaveInfo info_l;
		info_l.mainWave.steps = 6*60*100;
		info_l.mainWave.units = {{"square", 5}};
		pool_l.infos = {info_l};
		waves_l.push_back(pool_l);

		info_l.mainWave.steps = 6*60*100;
		info_l.mainWave.units = {{"triangle", 10}};
		pool_l.infos = {info_l};
		waves_l.push_back(pool_l);
	}

	std::vector<unsigned long> handles_l = getHandles(entityInfo_p, 0, "command_center");
	octopus::Vector waveTarget_l(124, 116);
	if(handles_l.size() == 1)
	{
		GodotEntityInfo const &cc_l = entityInfo_p[handles_l[0]];
		waveTarget_l = octopus::Vector(cc_l.x, cc_l.y);
	}

	std::list<WaveParam> params_l;
	for(size_t i = 0 ; i < waves_l.size() ; ++ i)
	{
		params_l.push_back({
			{octopus::Vector(10,10), octopus::Vector(240,10), octopus::Vector(10,240), octopus::Vector(240,240)},
			waveTarget_l,					// target
			0, 0, 0,						// position constraint to remove (none here)
			waves_l[i],						// wave options to be spawned
			nSpawnPoints_l
		});
	}

	WaveParam const &paramFirst_l = *params_l.begin();
	WaveInfo firstWave_l = rollWave(rand_p, paramFirst_l.wavePool);
	std::vector<octopus::Vector> rolledSpawns_l = rollSpawnPoints(paramFirst_l.spawnPoints, paramFirst_l.nSpawnPoints, rand_p);

	Trigger * triggerWave_l = new WaveSpawn(new ListenerStepCount(firstWave_l.earlyWave.steps), firstWave_l, rolledSpawns_l, true,
			lib_p, rand_p, params_l, player_p, defaultGenerator);

	spawners_l.push_back(new TriggerSpawn(triggerWave_l));

	Handle flyingCommandHandle_l(0);
	for(unsigned long i = 0 ; i < playerCount_p ; ++ i)
	{
		unsigned long playerIdx_l = 2+i;

		spawners_l.push_back(new PlayerAddBuildingModel(playerIdx_l, lib_p.getBuildingModel("barrack_square")));
		spawners_l.push_back(new PlayerAddBuildingModel(playerIdx_l, lib_p.getBuildingModel("barrack_circle")));
		spawners_l.push_back(new PlayerAddBuildingModel(playerIdx_l, lib_p.getBuildingModel("barrack_triangle")));
		spawners_l.push_back(new PlayerAddBuildingModel(playerIdx_l, lib_p.getBuildingModel("deposit")));
		spawners_l.push_back(new PlayerAddBuildingModel(playerIdx_l, lib_p.getBuildingModel("anchor")));

		fas::addBuildingPlayer(spawners_l, playerIdx_l, fas::allDivinities(), lib_p);
		spawners_l.push_back(new TriggerSpawn(new LoseTrigger(new ListenerEntityModelDied(&lib_p.getBuildingModel("command_center"), playerIdx_l))));
		// handles of command_center spawned by the level
		std::vector<unsigned long> handles_l = getHandles(entityInfo_p, playerIdx_l, "command_center");
		for(unsigned long ccHandle_l : handles_l)
		{
			spawners_l.push_back(new FlyingCommandSpawnStep(new TimerDamage(flyingCommandHandle_l++, anchorTickRate_l, 0, playerIdx_l, "Anchor", Handle(ccHandle_l))));
		}
		spawners_l.push_back(new TriggerSpawn(new AnchorTrigger(lib_p, rand_p, 150, playerIdx_l)));
	}

	return spawners_l;
}

AreaSpawnerCommand * createResourceNodeSpawnCommmand(Library &lib_p, RandomGenerator &rand_p, unsigned long x, unsigned long y, unsigned long size,
	size_t nbRes_p, size_t nbAnchorSpot_p, size_t nbUnits_p, size_t qtyRes_p, size_t qtyIrium_p, size_t nbDoubleSquares_p, size_t nbQuadSquares_p, size_t nbUSquares_p)
{
	std::list<AreaSpawn> spawners_l;

	Resource res2_l({0,0}, true, lib_p.getResourceModel("resource_bloc"));
	res2_l._resource = qtyRes_p;
	res2_l._player = 0;

	Resource res3_l({0,0}, true, lib_p.getResourceModel("resource_ether"));
	res3_l._resource = qtyRes_p;
	res3_l._player = 0;

	Resource res4_l({0,0}, true, lib_p.getResourceModel("resource_irium"));
	res4_l._resource = qtyIrium_p;
	res4_l._player = 0;

	Building tree_l({0,0}, true, lib_p.getBuildingModel("water_v_up"));
	tree_l._player = 0;

	Building anchorSpot_l({0,0}, true, lib_p.getBuildingModel("anchor_spot"));
	anchorSpot_l._player = 0;

	{
		AreaSpawn area_l;
		area_l.width = size;
		area_l.height = size;
		area_l.x = x;
		area_l.y = y;
		area_l.entities.emplace_back(new Resource(res3_l), nbRes_p);
		area_l.entities.emplace_back(new Resource(res2_l), nbRes_p);
		int nbTrees_l = rand_p.roll(1,2);
		for(int i = 0 ; i < nbTrees_l ; ++ i)
		{
			area_l.entities.emplace_back(new Building(tree_l), nbRes_p);
		}
		if(qtyIrium_p>0)
		{
			area_l.entities.emplace_back(new Resource(res4_l), 1);
		}
		if(nbAnchorSpot_p>0)
		{
			area_l.entities.emplace_back(new Building(anchorSpot_l), nbAnchorSpot_p);
		}
		spawners_l.push_back(area_l);
	}
	{
		AreaSpawn area_l;
		area_l.width = size-10;
		area_l.height = size-10;
		area_l.x = x+5;
		area_l.y = y+5;
		for(unsigned long c = 0 ; c < nbUnits_p ; ++ c)
		{
			Unit *unit_l = new Unit({0, 0}, false, lib_p.getUnitModel(genModelName(rand_p)));
			unit_l->_player = 1;
			area_l.entities.emplace_back(unit_l, 1);
		}
		for(unsigned long c = 0 ; c < nbDoubleSquares_p ; ++ c)
		{
			Unit *unit_l = new Unit({0, 0}, false, lib_p.getUnitModel("double_square_survival"));
			unit_l->_player = 1;
			area_l.entities.emplace_back(unit_l, 1);
		}
		for(unsigned long c = 0 ; c < nbQuadSquares_p ; ++ c)
		{
			Unit *unit_l = new Unit({0, 0}, false, lib_p.getUnitModel("quad_square_survival"));
			unit_l->_player = 1;
			area_l.entities.emplace_back(unit_l, 1);
		}
		for(unsigned long c = 0 ; c < nbUSquares_p ; ++ c)
		{
			Unit *unit_l = new Unit({0, 0}, false, lib_p.getUnitModel("U_square_survival"));
			unit_l->_player = 1;
			area_l.entities.emplace_back(unit_l, 1);
		}
		spawners_l.push_back(area_l);
	}

	return new AreaSpawnerCommand(rand_p, spawners_l);
}

std::list<Command *> DemoLevelCommands(Library &lib_p, RandomGenerator &rand_p, unsigned long playerCount_p, int difficulty_p)
{
	// resource on basic resources node based on difficulty
	size_t basicRes_l = difficulty_p < 3 ? 4000 : 2000;
	size_t advancedRes_l = difficulty_p < 3 ? 1000 : 500;

	// createResourceNodeSpawnCommmand(lib, rand, x, y, size, nb res, nb anchor, nb units, qty irium)
	std::list<Command *> commands_l {
		// cirle 1
		createResourceNodeSpawnCommmand(lib_p, rand_p, 154, 91, 18, 1*playerCount_p, 1*playerCount_p, 10, basicRes_l, advancedRes_l, 0, 0, 0),
		createResourceNodeSpawnCommmand(lib_p, rand_p, 108, 64, 19, 1*playerCount_p, 1*playerCount_p, 10, basicRes_l, advancedRes_l, 0, 0, 0),
		createResourceNodeSpawnCommmand(lib_p, rand_p, 71, 104, 14, 1*playerCount_p, 1*playerCount_p, 10, basicRes_l, advancedRes_l, 0, 0, 0),
		createResourceNodeSpawnCommmand(lib_p, rand_p, 108, 144, 18, 1*playerCount_p, 1*playerCount_p, 10, basicRes_l, advancedRes_l, 0, 0, 0),
		// cirle 2
		createResourceNodeSpawnCommmand(lib_p, rand_p, 44, 147, 22, 1*playerCount_p, 1*playerCount_p, 20, basicRes_l, 2*advancedRes_l, 0, 5, 0),
		createResourceNodeSpawnCommmand(lib_p, rand_p, 155, 170, 22, 1*playerCount_p, 1*playerCount_p, 20, basicRes_l, 2*advancedRes_l, 0, 5, 0),
		createResourceNodeSpawnCommmand(lib_p, rand_p, 162, 50, 21, 1*playerCount_p, 1*playerCount_p, 20, basicRes_l, 2*advancedRes_l, 0, 5, 0),
		createResourceNodeSpawnCommmand(lib_p, rand_p, 56, 60, 23, 1*playerCount_p, 1*playerCount_p, 20, basicRes_l, 2*advancedRes_l, 0, 5, 0),
		// cirle 3
		createResourceNodeSpawnCommmand(lib_p, rand_p, 102, 205, 27, 1*playerCount_p, 1*playerCount_p, 20, basicRes_l, 2*advancedRes_l, 5, 10, 0),
		createResourceNodeSpawnCommmand(lib_p, rand_p, 9, 101, 22, 1*playerCount_p, 1*playerCount_p, 20, basicRes_l, 2*advancedRes_l, 5, 10, 0),
		createResourceNodeSpawnCommmand(lib_p, rand_p, 106, 3, 28, 1*playerCount_p, 1*playerCount_p, 20, basicRes_l, 2*advancedRes_l, 5, 10, 0),
		createResourceNodeSpawnCommmand(lib_p, rand_p, 206, 115, 32, 1*playerCount_p, 1*playerCount_p, 20, basicRes_l, 2*advancedRes_l, 5, 10, 0),
		// cirle 4
		createResourceNodeSpawnCommmand(lib_p, rand_p, 207, 211, 26, 2*playerCount_p, 1*playerCount_p, 30, basicRes_l, 4*advancedRes_l, 5, 10, 3),
		createResourceNodeSpawnCommmand(lib_p, rand_p, 16, 207, 34, 2*playerCount_p, 1*playerCount_p, 30, basicRes_l, 4*advancedRes_l, 5, 10, 3),
		createResourceNodeSpawnCommmand(lib_p, rand_p, 6, 4, 25, 2*playerCount_p, 1*playerCount_p, 30, basicRes_l, 4*advancedRes_l, 5, 10, 3),
		createResourceNodeSpawnCommmand(lib_p, rand_p, 209, 12, 31, 2*playerCount_p, 1*playerCount_p, 30, basicRes_l, 4*advancedRes_l, 5, 10, 3),
	};

	return commands_l;
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/// 		read/write for Header		  ///
/////////////////////////////////////////////
/////////////////////////////////////////////

void writeWaveContentInfo(std::ofstream &file_p, WaveContentInfo const &info_p)
{
	file_p.write((char*)&info_p.steps, sizeof(info_p.steps));
	size_t size_l = info_p.units.size();
	file_p.write((char*)&size_l, sizeof(size_l));
	for(WaveUnitCount const &unitCount_l : info_p.units)
	{
		writeString(file_p, unitCount_l.model);
		file_p.write((char*)&unitCount_l.count, sizeof(unitCount_l.count));
	}
}

void readWaveContentInfo(std::ifstream &file_p, WaveContentInfo &info_p)
{
	file_p.read((char*)&info_p.steps, sizeof(info_p.steps));
	size_t size_l = 0;
	file_p.read((char*)&size_l, sizeof(size_l));
	for(size_t i = 0 ; i < size_l ; ++ i)
	{
		info_p.units.push_back(WaveUnitCount());
		info_p.units.back().model = readString(file_p);
		file_p.read((char*)&info_p.units.back().count, sizeof(info_p.units.back().count));
	}
}

void writeWavePoolInfo(std::ofstream &file_p, WavePoolInfo const &info_p)
{
	// write wave info
	size_t size_l = info_p.infos.size();
	file_p.write((char*)&size_l, sizeof(size_l));
	for(WaveInfo const &waveInfo_l : info_p.infos)
	{
		// write ugprades
		size_l = waveInfo_l.upgrades.size();
		file_p.write((char*)&size_l, sizeof(size_l));
		for(std::string const &up_l : waveInfo_l.upgrades)
		{
			writeString(file_p, up_l);
		}
		// write waves info
		writeWaveContentInfo(file_p, waveInfo_l.mainWave);
		writeWaveContentInfo(file_p, waveInfo_l.earlyWave);
		// write drop coef
		file_p.write((char*)&waveInfo_l.dropCoef, sizeof(waveInfo_l.dropCoef));
	}
}

void readWavePoolInfo(std::ifstream &file_p, WavePoolInfo &info_p)
{
	// read wave info
	size_t size_l = 0;
	file_p.read((char*)&size_l, sizeof(size_l));
	for(size_t i = 0 ; i < size_l ; ++ i)
	{
		WaveInfo waveInfo_l;
		// read upgrades
		size_t sizeUpgrades_l = 0;
		file_p.read((char*)&sizeUpgrades_l, sizeof(sizeUpgrades_l));
		for(size_t j = 0 ; j < sizeUpgrades_l ; ++ j)
		{
			std::string up_l = readString(file_p);
			waveInfo_l.upgrades.push_back(up_l);
		}
		// read waves info
		readWaveContentInfo(file_p, waveInfo_l.mainWave);
		readWaveContentInfo(file_p, waveInfo_l.earlyWave);
		// read drop coef
		file_p.read((char*)&waveInfo_l.dropCoef, sizeof(waveInfo_l.dropCoef));
		info_p.infos.push_back(waveInfo_l);
	}
}

/// @brief write header for classic arena level
void writeDemoLevelHeader(std::ofstream &file_p, DemoLevelHeader const &header_p)
{
	file_p.write((char*)&header_p.seed, sizeof(header_p.seed));
	file_p.write((char*)&header_p.player, sizeof(header_p.player));
	file_p.write((char*)&header_p.difficulty, sizeof(header_p.difficulty));
	file_p.write((char*)&header_p.player_count, sizeof(header_p.player_count));

	size_t size_l = header_p.tierWaveInfo.size();
	file_p.write((char*)&size_l, sizeof(size_l));

	for(WavePoolInfo const &info_l : header_p.tierWaveInfo)
	{
		writeWavePoolInfo(file_p, info_l);
	}
}

void readDemoLevelHeader(std::ifstream &file_p, DemoLevelHeader &header_r)
{
	file_p.read((char*)&header_r.seed, sizeof(header_r.seed));
	file_p.read((char*)&header_r.player, sizeof(header_r.player));
	file_p.read((char*)&header_r.difficulty, sizeof(header_r.difficulty));
	file_p.read((char*)&header_r.player_count, sizeof(header_r.player_count));

	size_t size_l = 0;
	file_p.read((char*)&size_l, sizeof(size_l));

	for(size_t i = 0 ; i < size_l ; ++ i)
	{
		WavePoolInfo info_l;
		readWavePoolInfo(file_p, info_l);
		header_r.tierWaveInfo.push_back(info_l);
	}
}

std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readDemoLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	std::vector<GodotEntityInfo> const &entityInfo_p,
	octopus::RandomGenerator * &rand_p, DemoLevelHeader &header_r)
{
	readDemoLevelHeader(file_p, header_r);

	delete rand_p;
	rand_p = new octopus::RandomGenerator(header_r.seed);

	std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > pair_l;
	pair_l.first = DemoLevelSteps(lib_p, *rand_p, header_r.tierWaveInfo, header_r.player, header_r.player_count, entityInfo_p, header_r.difficulty);
	pair_l.second = DemoLevelCommands(lib_p, *rand_p, 1, header_r.difficulty);
	return pair_l;
}

} // namespace level2
} // namespace godot
