#include "Level2.h"

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
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/player/PlayerAddBuildingModel.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/state/StateTemplePositionAddStep.hh"
#include "step/state/StateWinStep.hh"
#include "step/team/TeamVisionStep.hh"
#include "step/trigger/TriggerSpawn.hh"
#include "utils/Binary.hh"

// godot
#include "controller/step/CameraStep.h"
#include "controller/step/DialogStep.h"

#include "wave/WaveSpawn.h"
#include "level2/UtilsLevel2.h"

using namespace octopus;
using namespace fas;

namespace godot
{
namespace level2
{

std::list<Steppable *> WaveLevelSteps(Library &lib_p, RandomGenerator &rand_p, std::vector<WavePoolInfo> const &waveInfo_p, unsigned long player_p, unsigned long playerCount_p)
{
	loadMinimalModels(lib_p);

	// load divinity library
	fas::loadLibrary(lib_p);

	Building building_l({20, 50}, true, lib_p.getBuildingModel("command_center"));
	Unit unit_l({ 17, 50 }, false, lib_p.getUnitModel("worker"));

	long anchor_l = 420;
	std::map<std::string, Fixed> mapRes_l;
	mapRes_l["bloc"] = -200;
	mapRes_l["ether"] = -200;
	mapRes_l["Anchor"] = -anchor_l;

	std::vector<WavePoolInfo> waves_l = waveInfo_p;
	if(waves_l.empty())
	{
		WavePoolInfo pool_l;

		WaveInfo info_l;
		info_l.mainWave.steps = 6*60*100;
		info_l.mainWave.units = {{"square", 10}};
		pool_l.infos = {info_l};
		waves_l.push_back(pool_l);

		info_l.mainWave.steps = 6*60*100;
		info_l.mainWave.units = {{"triangle", 50}};
		pool_l.infos = {info_l};
		waves_l.push_back(pool_l);

		info_l.mainWave.steps = 6*60*100;
		info_l.mainWave.units = {{"circle", 100}};
		pool_l.infos = {info_l};
		waves_l.push_back(pool_l);

		info_l.mainWave.steps = 6*60*100;
		info_l.mainWave.units = {{"square", 200}};
		pool_l.infos = {info_l};
		waves_l.push_back(pool_l);

		info_l.mainWave.steps = 6*60*100;
		info_l.mainWave.units = {{"triangle", 300}};
		pool_l.infos = {info_l};
		waves_l.push_back(pool_l);

		info_l.mainWave.steps = 6*60*100;
		info_l.mainWave.units = {{"circle", 1000}};
		pool_l.infos = {info_l};
		waves_l.push_back(pool_l);
	}

	// the y coordinate where every wave will spawn (depends on the number of player)
	octopus::Fixed ySpawnWave_l = 50;
	if(playerCount_p==2)
	{
		ySpawnWave_l = 65;
	}
	else if(playerCount_p==3)
	{
		ySpawnWave_l = 80;
	}
	// if more than 3 players we stick to one player (more is not supported)
	else
	{
		playerCount_p = 1;
	}

	std::list<WaveParam> params_l;
	for(uint32_t i = 0 ; i < waves_l.size() ; ++ i)
	{
		if(i == 0)
			params_l.push_back({{octopus::Vector(60,ySpawnWave_l)}, octopus::Vector(20,ySpawnWave_l), 40, 40, 60, waves_l[i]});
		else if(i==1)
			params_l.push_back({{octopus::Vector(100,ySpawnWave_l)}, octopus::Vector(20,ySpawnWave_l), 80, 40, 60, waves_l[i]});
		else if(i==2)
			params_l.push_back({{octopus::Vector(140,ySpawnWave_l)}, octopus::Vector(20,ySpawnWave_l), 120, 40, 60, waves_l[i]});
		else if(i==3)
			params_l.push_back({{octopus::Vector(180,ySpawnWave_l)}, octopus::Vector(20,ySpawnWave_l), 160, 40, 60, waves_l[i]});
		else
			params_l.push_back({{octopus::Vector(240,ySpawnWave_l)}, octopus::Vector(20,ySpawnWave_l), 160, 40, 60, waves_l[i]});
	}

	WaveInfo firstWave_l = rollWave(rand_p, waves_l[0]);

	Trigger * triggerWave_l = new WaveSpawn(new ListenerStepCount(firstWave_l.earlyWave.steps), firstWave_l, {octopus::Vector(60,ySpawnWave_l)}, true,
			lib_p, rand_p, params_l, player_p, {}, defaultGenerator, {});

	Handle handle_l(0);
	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),	// main player
		new PlayerSpawnStep(1, 1),	// enemy
		new PlayerSpawnStep(2, 2),	// neutral
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("barrack_square")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("barrack_circle")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("barrack_triangle")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("deposit")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("anchor")),
		new PlayerSpendResourceStep(0, mapRes_l),
		new BuildingSpawnStep(handle_l++, building_l, true),
		new UnitSpawnStep(handle_l++, unit_l),
		new UnitSpawnStep(handle_l++, unit_l),
		new UnitSpawnStep(handle_l++, unit_l),
		new UnitSpawnStep(handle_l++, unit_l),
		new UnitSpawnStep(handle_l++, unit_l),
		new UnitSpawnStep(handle_l++, unit_l),
		new UnitSpawnStep(handle_l++, unit_l),
		new UnitSpawnStep(handle_l++, unit_l),
		new UnitSpawnStep(handle_l++, unit_l),
		new UnitSpawnStep(handle_l++, unit_l),
		new TriggerSpawn(triggerWave_l),
		new TriggerSpawn(new LoseTrigger(new ListenerEntityModelDied(&lib_p.getBuildingModel("command_center"), 0))),
		new TriggerSpawn(new AnchorTrigger(lib_p, rand_p, 150)),
		new FlyingCommandSpawnStep(new TimerDamage(100, 0, 0, "Anchor", Handle(0))),
		new godot::CameraStep(to_int(building_l._pos.x), to_int(building_l._pos.y), 0),
		new godot::DialogStep("leve1_intro"),
	};

	std::vector<unsigned long> players_l {0};
	// add players
	for(unsigned long i = 1 ; i < playerCount_p ; ++ i)
	{
		unsigned long player_l = 2+i;
		players_l.push_back(player_l);

		spawners_l.push_back(new PlayerSpawnStep(player_l, 0));
		spawners_l.push_back(new PlayerAddBuildingModel(player_l, lib_p.getBuildingModel("barrack_square")));
		spawners_l.push_back(new PlayerAddBuildingModel(player_l, lib_p.getBuildingModel("barrack_circle")));
		spawners_l.push_back(new PlayerAddBuildingModel(player_l, lib_p.getBuildingModel("barrack_triangle")));
		spawners_l.push_back(new PlayerAddBuildingModel(player_l, lib_p.getBuildingModel("deposit")));
		spawners_l.push_back(new PlayerAddBuildingModel(player_l, lib_p.getBuildingModel("anchor")));
		spawners_l.push_back(new PlayerSpendResourceStep(player_l, mapRes_l));

		// update player of spawn steps
		building_l._player = player_l;
		unit_l._player = player_l;
		building_l._pos.y = 50 + 30*i;
		unit_l._pos.y = 50 + 30*i;

		Handle ccHandle_l = handle_l;
		spawners_l.push_back(new BuildingSpawnStep(handle_l++, building_l, true));
		for(unsigned long j = 0 ; j < 10; ++ j)
		{
			spawners_l.push_back(new UnitSpawnStep(handle_l++, unit_l));
		}
		spawners_l.push_back(new TriggerSpawn(new LoseTrigger(new ListenerEntityModelDied(&lib_p.getBuildingModel("command_center"), player_l))));
		spawners_l.push_back(new FlyingCommandSpawnStep(new TimerDamage(100, 0, 0, "Anchor", ccHandle_l)));

		spawners_l.push_back(new godot::CameraStep(to_int(building_l._pos.x), to_int(building_l._pos.y), player_l));
	}

	// y length depends on player count
	unsigned long yLength_l = 100;
	unsigned long yStartGate_l = 40;
	unsigned long gateSize_l = 20;
	unsigned long lastGateIncrease_l = 30;
	if(playerCount_p==2)
	{
		yLength_l = 130;
		yStartGate_l = 55;
	}
	else if(playerCount_p==3)
	{
		yLength_l = 160;
		yStartGate_l = 70;
	}

	// zone 1
	std::list<Steppable *> zone_l = createWallSpawners(lib_p, 0, 40, yLength_l, yStartGate_l, yStartGate_l+gateSize_l, handle_l, players_l);
	spawners_l.insert(spawners_l.end(), zone_l.begin(), zone_l.end());

	// zone 2
	zone_l = createWallSpawners(lib_p, 40, 80, yLength_l, yStartGate_l, yStartGate_l+gateSize_l, handle_l, players_l);
	spawners_l.insert(spawners_l.end(), zone_l.begin(), zone_l.end());

	// zone 3
	zone_l = createWallSpawners(lib_p, 80, 120, yLength_l, yStartGate_l, yStartGate_l+gateSize_l, handle_l, players_l);
	spawners_l.insert(spawners_l.end(), zone_l.begin(), zone_l.end());

	// zone 4
	zone_l = createWallSpawners(lib_p, 120, 160, yLength_l, yStartGate_l, yStartGate_l+gateSize_l, handle_l, players_l);
	spawners_l.insert(spawners_l.end(), zone_l.begin(), zone_l.end());

	// zone 5
	zone_l = createWallSpawners(lib_p, 160, 220, yLength_l, yStartGate_l-lastGateIncrease_l, yStartGate_l+gateSize_l+lastGateIncrease_l, handle_l, players_l);
	spawners_l.insert(spawners_l.end(), zone_l.begin(), zone_l.end());

	return spawners_l;
}

AreaSpawnerCommand * createArenaSpawnCommmand(Library &lib_p, RandomGenerator &rand_p, unsigned long x, unsigned long y, unsigned long size,
	uint32_t nbRes_p, uint32_t nbAnchorSpot_p, uint32_t nbUnits_p, uint32_t qtyIrium_p)
{
	std::list<AreaSpawn> spawners_l;

	Resource res2_l({0,0}, true, lib_p.getResourceModel("resource_bloc"));
	res2_l._resource = 2000.;
	res2_l._player = 2;

	Resource res3_l({0,0}, true, lib_p.getResourceModel("resource_ether"));
	res3_l._resource = 2000.;
	res3_l._player = 2;

	Resource res4_l({0,0}, true, lib_p.getResourceModel("resource_irium"));
	res4_l._resource = qtyIrium_p;
	res4_l._player = 2;

	Building tree_l({0,0}, true, lib_p.getBuildingModel("water_v_up"));
	tree_l._player = 2;

	Building anchorSpot_l({0,0}, true, lib_p.getBuildingModel("anchor_spot"));
	anchorSpot_l._player = 2;

	{
		AreaSpawn area_l;
		area_l.width = size;
		area_l.height = size;
		area_l.x = x;
		area_l.y = y;
		area_l.entities.emplace_back(new Resource(res3_l), nbRes_p);
		area_l.entities.emplace_back(new Resource(res2_l), nbRes_p);
		int nbTrees_l = rand_p.roll(2,5);
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
		spawners_l.push_back(area_l);
	}

	return new AreaSpawnerCommand(rand_p, spawners_l);
}

std::list<Command *> WaveLevelCommands(Library &lib_p, RandomGenerator &rand_p, unsigned long playerCount_p)
{
	unsigned long yPosTopArea_l = 5;
	unsigned long yPosBotArea_l = 65;
	if(playerCount_p==2)
	{
		yPosBotArea_l = 95;
	}
	else if(playerCount_p==3)
	{
		yPosBotArea_l = 125;
	}

	// createArenaSpawnCommmand(lib, rand, x, y, size, nb res, nb anchor, nb units, qty irium)
	std::list<Command *> commands_l {
		// zone 0
		createArenaSpawnCommmand(lib_p, rand_p, 5, yPosTopArea_l, 30, 1*playerCount_p, 1*playerCount_p, 0, 0),
		createArenaSpawnCommmand(lib_p, rand_p, 5, yPosBotArea_l, 30, 1*playerCount_p, 1*playerCount_p, 0, 0),
		// zone 1
		createArenaSpawnCommmand(lib_p, rand_p, 45, yPosTopArea_l, 30, 1*playerCount_p, 1*playerCount_p, 20, 500),
		createArenaSpawnCommmand(lib_p, rand_p, 45, yPosBotArea_l, 30, 1*playerCount_p, 1*playerCount_p, 20, 500),
		// zone 2
		createArenaSpawnCommmand(lib_p, rand_p, 85, yPosTopArea_l, 30, 1*playerCount_p, 1*playerCount_p, 30, 1000),
		createArenaSpawnCommmand(lib_p, rand_p, 85, yPosBotArea_l, 30, 1*playerCount_p, 1*playerCount_p, 30, 1000),
		// zone 3
		createArenaSpawnCommmand(lib_p, rand_p, 125, yPosTopArea_l, 30, 2*playerCount_p, 1*playerCount_p, 40, 2000),
		createArenaSpawnCommmand(lib_p, rand_p, 125, yPosBotArea_l, 30, 2*playerCount_p, 1*playerCount_p, 40, 2000),
		// zone 4
		createArenaSpawnCommmand(lib_p, rand_p, 165, yPosTopArea_l, 30, 2*playerCount_p, 1*playerCount_p, 50, 2000),
		createArenaSpawnCommmand(lib_p, rand_p, 165, yPosBotArea_l, 30, 2*playerCount_p, 1*playerCount_p, 50, 2000),
	};

	// players area
	for(unsigned long i = 0 ; i < playerCount_p ; ++ i)
	{
		commands_l.push_back(createArenaSpawnCommmand(lib_p, rand_p, 10, 40+30*i, 20, 1, 0, 0, 0));
		commands_l.push_back(createArenaSpawnCommmand(lib_p, rand_p, 50, 40+30*i, 20, 1, 1, 10, 500));
		commands_l.push_back(createArenaSpawnCommmand(lib_p, rand_p, 90, 40+30*i, 20, 1, 1, 20, 1000));
		commands_l.push_back(createArenaSpawnCommmand(lib_p, rand_p, 130, 40+30*i, 20, 2, 1, 30, 2000));
		commands_l.push_back(createArenaSpawnCommmand(lib_p, rand_p, 170, 40+30*i, 20, 2, 1, 40, 2000));
	}
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
	uint32_t size_l = info_p.units.size();
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
	uint32_t size_l = 0;
	file_p.read((char*)&size_l, sizeof(size_l));
	for(uint32_t i = 0 ; i < size_l ; ++ i)
	{
		info_p.units.push_back(WaveUnitCount());
		info_p.units.back().model = readString(file_p);
		file_p.read((char*)&info_p.units.back().count, sizeof(info_p.units.back().count));
	}
}

void writeWavePoolInfo(std::ofstream &file_p, WavePoolInfo const &info_p)
{
	// write wave info
	uint32_t size_l = info_p.infos.size();
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
	uint32_t size_l = 0;
	file_p.read((char*)&size_l, sizeof(size_l));
	for(uint32_t i = 0 ; i < size_l ; ++ i)
	{
		WaveInfo waveInfo_l;
		// read upgrades
		uint32_t sizeUpgrades_l = 0;
		file_p.read((char*)&sizeUpgrades_l, sizeof(sizeUpgrades_l));
		for(uint32_t j = 0 ; j < sizeUpgrades_l ; ++ j)
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
void writeWaveLevelHeader(std::ofstream &file_p, WaveLevelHeader const &header_p)
{
	file_p.write((char*)&header_p.seed, sizeof(header_p.seed));
	file_p.write((char*)&header_p.player, sizeof(header_p.player));

	uint32_t size_l = header_p.tierWaveInfo.size();
	file_p.write((char*)&size_l, sizeof(size_l));

	for(WavePoolInfo const &info_l : header_p.tierWaveInfo)
	{
		writeWavePoolInfo(file_p, info_l);
	}
}

void readWaveLevelHeader(std::ifstream &file_p, WaveLevelHeader &header_r)
{
	file_p.read((char*)&header_r.seed, sizeof(header_r.seed));
	file_p.read((char*)&header_r.player, sizeof(header_r.player));

	uint32_t size_l = 0;
	file_p.read((char*)&size_l, sizeof(size_l));

	for(uint32_t i = 0 ; i < size_l ; ++ i)
	{
		WavePoolInfo info_l;
		readWavePoolInfo(file_p, info_l);
		header_r.tierWaveInfo.push_back(info_l);
	}
}

std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readWaveLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p, WaveLevelHeader &header_r)
{
	readWaveLevelHeader(file_p, header_r);

	delete rand_p;
	rand_p = new octopus::RandomGenerator(header_r.seed);

	std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > pair_l;
	pair_l.first = WaveLevelSteps(lib_p, *rand_p, header_r.tierWaveInfo, header_r.player, 1);
	pair_l.second = WaveLevelCommands(lib_p, *rand_p, 1);
	return pair_l;
}

} // namespace level2
} // namespace godot
