#include "Level2.h"

#include <iostream>
#include <fstream>
#include <random>

// fas
#include "library/model/AnchorTrigger.hh"
#include "library/model/ModelLoader.hh"
#include "library/model/TimerDamage.hh"

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

#include "level2/WaveSpawn.h"
#include "level2/UtilsLevel2.h"

using namespace octopus;

namespace godot
{
namespace level2
{

class VisionTrigger : public octopus::OneShotTrigger
{
public:
	VisionTrigger(unsigned long timer_p) : OneShotTrigger({new octopus::ListenerStepCount(timer_p)}) {}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long, TriggerData const &) const override
	{
		step_p.addSteppable(new godot::DialogStep("show anchor"));
	}
};

std::list<Steppable *> WaveLevelSteps(Library &lib_p, RandomGenerator &rand_p)
{
	loadMinimalModels(lib_p);

	Building building_l({20, 10}, true, lib_p.getBuildingModel("command_center"));
	Unit unit_l({ 17, 10 }, false, lib_p.getUnitModel("worker"));

	long anchor_l = 420;
	std::map<std::string, Fixed> mapRes_l;
	mapRes_l["bloc"] = -200;
	mapRes_l["ether"] = -200;
	mapRes_l["Anchor"] = -anchor_l;
	unsigned long timeTriggerAnchor_l = (anchor_l-60)*100;

	std::list<WaveParam> params_l;
	params_l.push_back({octopus::Vector(60,30), 6*60*100, 10, 40, 0});
	params_l.push_back({octopus::Vector(100,30), 6*60*100, 50, 80, 0});
	params_l.push_back({octopus::Vector(140,30), 6*60*100, 100, 120, 0});
	params_l.push_back({octopus::Vector(180,30), 6*60*100, 200, 160, 0});
	params_l.push_back({octopus::Vector(220,30), 6*60*100, 300, 160, 0});
	params_l.push_back({octopus::Vector(220,30), 6*60*100, 1000, 160, 0});

	Trigger * triggerWave_l = new WaveSpawn(new ListenerStepCount(params_l.front().stepWait), lib_p, rand_p, params_l, defaultGenerator);
	Trigger * triggerLose_l = new LoseTrigger(new ListenerEntityModelDied(&lib_p.getBuildingModel("command_center"), 0));

	Handle handle_l = 0;
	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new PlayerSpawnStep(2, 2),
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
		new TriggerSpawn(triggerLose_l),
		new TriggerSpawn(new AnchorTrigger(lib_p, rand_p, 150)),
		new TriggerSpawn(new VisionTrigger(timeTriggerAnchor_l)),
		new FlyingCommandSpawnStep(new TimerDamage(0, 100, 0, 0, "Anchor", 0)),
		new godot::CameraStep(to_int(building_l._pos.x), to_int(building_l._pos.y)),
		new godot::DialogStep("leve1_intro"),
	};

	// zone 1
	std::list<Steppable *> zone_l = createWallSpawners(lib_p, 40, 100, 20, 40, handle_l);
	spawners_l.insert(spawners_l.end(), zone_l.begin(), zone_l.end());

	// zone 2
	zone_l = createWallSpawners(lib_p, 80, 100, 20, 40, handle_l);
	spawners_l.insert(spawners_l.end(), zone_l.begin(), zone_l.end());

	// zone 3
	zone_l = createWallSpawners(lib_p, 120, 100, 20, 40, handle_l);
	spawners_l.insert(spawners_l.end(), zone_l.begin(), zone_l.end());

	// zone 4
	zone_l = createWallSpawners(lib_p, 160, 100, 20, 40, handle_l);
	spawners_l.insert(spawners_l.end(), zone_l.begin(), zone_l.end());

	// zone 5
	zone_l = createWallSpawners(lib_p, 200, 100, 20, 40, handle_l);
	spawners_l.insert(spawners_l.end(), zone_l.begin(), zone_l.end());

	return spawners_l;
}

AreaSpawnerCommand * createArenaSpawnCommmand(Library &lib_p, RandomGenerator &rand_p, unsigned long x, unsigned long y, unsigned long size,
	size_t nbRes_p, size_t nbAnchorSpot_p, size_t nbUnits_p, size_t qtyIrium_p)
{
	std::list<AreaSpawn> spawners_l;

	Resource res2_l({0,0}, true, lib_p.getEntityModel("resource_food"));
	res2_l._type = "bloc";
	res2_l._resource = 2000.;
	res2_l._player = 2;

	Resource res3_l({0,0}, true, lib_p.getEntityModel("resource_steel"));
	res3_l._type = "ether";
	res3_l._resource = 2000.;
	res3_l._player = 2;

	Resource res4_l({0,0}, true, lib_p.getEntityModel("resource_irium"));
	res4_l._type = "irium";
	res4_l._resource = qtyIrium_p;
	res4_l._player = 2;

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

std::list<Command *> WaveLevelCommands(Library &lib_p, RandomGenerator &rand_p)
{
	// createArenaSpawnCommmand(lib, rand, x, y, size, nb res, nb anchor, nb units, qty irium)
	std::list<Command *> commands_l {
		// zone 0
		createArenaSpawnCommmand(lib_p, rand_p, 10, 10, 20, 1, 1, 10, 0),
		createArenaSpawnCommmand(lib_p, rand_p, 10, 70, 20, 1, 0, 0, 0),
		createArenaSpawnCommmand(lib_p, rand_p, 10, 70, 20, 1, 1, 10, 0),
		// zone 1
		createArenaSpawnCommmand(lib_p, rand_p, 50, 10, 20, 1, 1, 20, 500),
		createArenaSpawnCommmand(lib_p, rand_p, 50, 40, 20, 1, 1, 10, 500),
		createArenaSpawnCommmand(lib_p, rand_p, 50, 70, 20, 1, 1, 20, 500),
		// zone 2
		createArenaSpawnCommmand(lib_p, rand_p, 90, 10, 20, 1, 1, 30, 1000),
		createArenaSpawnCommmand(lib_p, rand_p, 90, 40, 20, 1, 1, 20, 1000),
		createArenaSpawnCommmand(lib_p, rand_p, 90, 70, 20, 1, 1, 30, 1000),
		// zone 3
		createArenaSpawnCommmand(lib_p, rand_p, 130, 10, 20, 1, 2, 40, 2000),
		createArenaSpawnCommmand(lib_p, rand_p, 130, 40, 20, 1, 2, 30, 2000),
		createArenaSpawnCommmand(lib_p, rand_p, 130, 70, 20, 1, 2, 40, 2000),
		// zone 4
		createArenaSpawnCommmand(lib_p, rand_p, 170, 10, 20, 1, 2, 50, 2000),
		createArenaSpawnCommmand(lib_p, rand_p, 170, 40, 20, 1, 2, 40, 2000),
		createArenaSpawnCommmand(lib_p, rand_p, 170, 70, 20, 1, 2, 50, 2000),
	};

	return commands_l;
}

/// @brief write header for classic arena level
void writeWaveLevelHeader(std::ofstream &file_p, WaveLevelHeader const &header_p)
{
    file_p.write((char*)&header_p.seed, sizeof(header_p.seed));
}

/// @brief read header for classic arena level and return a pair of steppable and command
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readWaveLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p, WaveLevelHeader &header_r)
{
    file_p.read((char*)&header_r.seed, sizeof(header_r.seed));

	delete rand_p;
	rand_p = new octopus::RandomGenerator(header_r.seed);

	std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > pair_l;
	pair_l.first = WaveLevelSteps(lib_p, *rand_p);
	pair_l.second = WaveLevelCommands(lib_p, *rand_p);
	return pair_l;
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/// 				Triggers			  ///
/////////////////////////////////////////////
/////////////////////////////////////////////

LoseTrigger::LoseTrigger(Listener * listener_p) : OneShotTrigger({listener_p}) {}

void LoseTrigger::trigger(State const &state_p, Step &step_p, unsigned long, octopus::TriggerData const &) const
{
	step_p.addSteppable(new StateWinStep(state_p.isOver(), state_p.hasWinningTeam(), state_p.getWinningTeam(), 1));
}

} // namespace level2
} // namespace godot
