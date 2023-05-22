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
	VisionTrigger(unsigned long timer_p, int x, int y, octopus::VisionPattern const &pattern_p) : OneShotTrigger({new octopus::ListenerStepCount(timer_p)}), _x(x), _y(y), _pattern(pattern_p) {}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long, TriggerData const &) const override
	{
		step_p.addSteppable(new octopus::TeamVisionStep(0, _pattern, true, true));
		step_p.addSteppable(new octopus::TeamVisionStep(0, _pattern, true, false));

		step_p.addSteppable(new godot::CameraStep(_x, _y));
		step_p.addSteppable(new godot::DialogStep("show anchor"));
	}
private:
	octopus::VisionPattern const _pattern;
	int const _x;
	int const _y;
};

std::list<Steppable *> WaveLevelSteps(Library &lib_p, RandomGenerator &rand_p)
{
	loadMinimalModels(lib_p);

	Building building_l({20, 10}, true, lib_p.getBuildingModel("command_center"));
	Unit unit_l({ 17, 10 }, false, lib_p.getUnitModel("worker"));

	Resource res1_l({15,12}, true, lib_p.getEntityModel("resource_food"));
	res1_l._type = "bloc";
	res1_l._resource = 2000.;
	res1_l._player = 2;

	Resource res3_l({15,8}, true, lib_p.getEntityModel("resource_steel"));
	res3_l._type = "ether";
	res3_l._resource = 2000.;
	res3_l._player = 2;

	long anchor_l = 420;
	std::map<std::string, Fixed> mapRes_l;
	mapRes_l["bloc"] = -200;
	mapRes_l["ether"] = -200;
	mapRes_l["Anchor"] = -anchor_l;
	unsigned long visionTrigger_l = (anchor_l-60)*100;

	std::list<WaveParam> params_l;
	params_l.push_back({octopus::Vector(60,10), 3*60*100, 10, 45, 30});
	params_l.push_back({octopus::Vector(90,35), 5*60*100, 50, 75, 60});
	params_l.push_back({octopus::Vector(90,35), 5*60*100, 100, 105, 90});
	params_l.push_back({octopus::Vector(90,35), 5*60*100, 200, 135, 120});
	params_l.push_back({octopus::Vector(90,35), 5*60*100, 300, 135, 120});

	Trigger * triggerWave_l = new WaveSpawn(new ListenerStepCount(params_l.front().stepWait), lib_p, rand_p, params_l, defaultGenerator);
	Trigger * triggerLose_l = new LoseTrigger(new ListenerEntityModelDied(&lib_p.getBuildingModel("command_center"), 0));

	Building anchorSpot_l({60,10}, true, lib_p.getBuildingModel("anchor_spot"));
	anchorSpot_l._player = 2;
    octopus::PatternHandler handler_l;
	octopus::VisionPattern pattern_l = handler_l.getPattern(10);
	for(std::pair<long, long> &pair_l : pattern_l)
	{
		pair_l.first += to_int(anchorSpot_l._pos.x);
		pair_l.second += to_int(anchorSpot_l._pos.y);
	}
	Handle handle_l = 0;
	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new PlayerSpawnStep(2, 2),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("command_center")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("barrack_square")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("barrack_circle")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("barrack_triangle")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("deposit")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("anchor")),
		new PlayerSpendResourceStep(0, mapRes_l),
		new BuildingSpawnStep(handle_l++, building_l, true),
		new ResourceSpawnStep(handle_l++, res1_l),
		new ResourceSpawnStep(handle_l++, res3_l),
		new BuildingSpawnStep(handle_l++, anchorSpot_l, true),
		new StateTemplePositionAddStep(anchorSpot_l._pos),
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
		new TriggerSpawn(new AnchorTrigger(lib_p, rand_p, 60)),
		new TriggerSpawn(new VisionTrigger(visionTrigger_l, to_int(anchorSpot_l._pos.x), to_int(anchorSpot_l._pos.y), pattern_l)),
		new FlyingCommandSpawnStep(new TimerDamage(0, 100, 0, 0, "Anchor", 0)),
		new godot::CameraStep(to_int(building_l._pos.x), to_int(building_l._pos.y)),
		new godot::DialogStep("leve1_intro"),
	};

	// zone 1
	std::list<Steppable *> zone_l = createWallSpawners(lib_p, 45, 30, 4, 14, handle_l);
	spawners_l.insert(spawners_l.end(), zone_l.begin(), zone_l.end());

	// zone 2
	zone_l = createWallSpawners(lib_p, 75, 60, 40, 50, handle_l);
	spawners_l.insert(spawners_l.end(), zone_l.begin(), zone_l.end());

	// zone 3
	zone_l = createWallSpawners(lib_p, 105, 90, 70, 80, handle_l);
	spawners_l.insert(spawners_l.end(), zone_l.begin(), zone_l.end());

	// zone 4
	zone_l = createWallSpawners(lib_p, 135, 120, 100, 110, handle_l);
	spawners_l.insert(spawners_l.end(), zone_l.begin(), zone_l.end());

	return spawners_l;
}

AreaSpawnerCommand * createArenaSpawnCommmand(Library &lib_p, RandomGenerator &rand_p, unsigned long x, unsigned long y, unsigned long size,
	size_t nbRes_p, size_t nbAnchorSpot_p, size_t nbUnits_p)
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
	res4_l._resource = 300.;
	res4_l._player = 2;

	Building anchorSpot_l({0,0}, true, lib_p.getBuildingModel("anchor_spot"));
	anchorSpot_l._player = 2;

	{
		AreaSpawn area_l;
		area_l.size = size;
		area_l.x = x;
		area_l.y = y;
		area_l.entities.emplace_back(new Resource(res3_l), nbRes_p);
		area_l.entities.emplace_back(new Resource(res2_l), nbRes_p);
		area_l.entities.emplace_back(new Resource(res4_l), 1);
		if(nbAnchorSpot_p>0)
		{
			area_l.entities.emplace_back(new Building(anchorSpot_l), nbAnchorSpot_p);
		}
		spawners_l.push_back(area_l);
	}
	{
		AreaSpawn area_l;
		area_l.size = size-10;
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
	std::list<Command *> commands_l {
		// zone 1
		createArenaSpawnCommmand(lib_p, rand_p, 50, 0, 20, 1, 0, 20),
		createArenaSpawnCommmand(lib_p, rand_p, 50, 35, 20, 1, 1, 30),
		createArenaSpawnCommmand(lib_p, rand_p, 15, 35, 20, 1, 1, 40),
		// zone 2
		createArenaSpawnCommmand(lib_p, rand_p, 80, 30, 20, 1, 1, 20),
		createArenaSpawnCommmand(lib_p, rand_p, 80, 65, 20, 1, 1, 30),
		createArenaSpawnCommmand(lib_p, rand_p, 45, 65, 20, 1, 1, 40),
	};

	return commands_l;
}

/// @brief write header for classic arena level
void writeWaveLevelHeader(std::ofstream &file_p, int seed_p)
{
    file_p.write((char*)&seed_p, sizeof(seed_p));
}

/// @brief read header for classic arena level and return a pair of steppable and command
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readWaveLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p)
{
	int seed_l;
    file_p.read((char*)&seed_l, sizeof(seed_l));

	delete rand_p;
	rand_p = new octopus::RandomGenerator(seed_l);

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
