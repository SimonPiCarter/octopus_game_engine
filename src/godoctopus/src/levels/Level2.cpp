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
#include "step/state/StateAddConstraintPositionStep.hh"
#include "step/state/StateRemoveConstraintPositionStep.hh"
#include "step/state/StateTemplePositionAddStep.hh"
#include "step/state/StateWinStep.hh"
#include "step/team/TeamVisionStep.hh"
#include "step/trigger/TriggerSpawn.hh"

// godot
#include "controller/step/CameraStep.h"
#include "controller/step/DialogStep.h"
#include "controller/step/WaveStep.h"

using namespace octopus;

namespace godot
{
namespace level2
{

std::vector<octopus::Steppable*> defaultGenerator() { return {new WaveStep()}; }

std::string genModelName(RandomGenerator &gen_p)
{
	std::string model_l = "square";
	int random_l = gen_p.roll(0, 2);
	if(random_l==1)
	{
		model_l = "triangle";
	}
	else if(random_l==2)
	{
		model_l = "circle";
	}
	return model_l;
}


class VisionTrigger : public octopus::OneShotTrigger
{
public:
	VisionTrigger(int x, int y, octopus::VisionPattern const &pattern_p) : OneShotTrigger({new octopus::ListenerStepCount(36000)}), _x(x), _y(y), _pattern(pattern_p) {}

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
	// 5 waves every 5 minutes
	unsigned long waveCount_p = 5;
	unsigned long stepCount_p = 5*60*100;
	unsigned long worldSize_p = 250;

	loadModels(lib_p);

	Building building_l({20, 10}, true, lib_p.getBuildingModel("command_center"));
	Unit unit_l({ 17, 10 }, false, lib_p.getUnitModel("worker"));

	/// TEMP
	Unit square_l({ 17, 10 }, false, lib_p.getUnitModel("square"));


	Resource res1_l({15,12}, true, lib_p.getEntityModel("resource_food"));
	res1_l._type = ResourceType::Food;
	res1_l._resource = 2000.;
	res1_l._player = 2;

	Resource res3_l({15,8}, true, lib_p.getEntityModel("resource_steel"));
	res3_l._type = ResourceType::Steel;
	res3_l._resource = 2000.;
	res3_l._player = 2;

	std::map<ResourceType, double> mapRes_l;
	mapRes_l[octopus::ResourceType::Food] = -200;
	mapRes_l[octopus::ResourceType::Steel] = -200;
	mapRes_l[octopus::ResourceType::Anchor] = -420;

	Trigger * triggerWave_l = new WaveSpawn(new ListenerStepCount(stepCount_p), lib_p, rand_p, 1, stepCount_p, waveCount_p, worldSize_p, defaultGenerator);
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
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("barrack")),
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
		new UnitSpawnStep(handle_l++, square_l),
		new TriggerSpawn(triggerWave_l),
		new TriggerSpawn(triggerLose_l),
		new TriggerSpawn(new AnchorTrigger(lib_p, rand_p, 60)),
		new TriggerSpawn(new VisionTrigger(to_int(anchorSpot_l._pos.x), to_int(anchorSpot_l._pos.y), pattern_l)),
		new FlyingCommandSpawnStep(new TimerDamage(0, 100, 0, 0, octopus::ResourceType::Anchor, 0)),
		new StateAddConstraintPositionStep(0, 45, 0, 40, true, true),
		new StateAddConstraintPositionStep(0, 30, 0, 45, true, false),
		new godot::CameraStep(to_int(building_l._pos.x), to_int(building_l._pos.y)),
		new godot::DialogStep("leve1_intro"),
	};

	// create walls for zone 1
	for(size_t i = 0 ; i < 32 ; i+=2)
	{
		if(i > 5 && i < 14)
		{
			continue;
		}
		Building building_l({46, i}, true, lib_p.getBuildingModel("barrack"));
		building_l._player = 1;
		spawners_l.push_back(new BuildingSpawnStep(handle_l++, building_l, true));
	}
	for(size_t i = 0 ; i < 45 ; i+=2)
	{
		Building building_l({i, 31}, true, lib_p.getBuildingModel("barrack"));
		building_l._player = 1;
		spawners_l.push_back(new BuildingSpawnStep(handle_l++, building_l, true));
	}

	return spawners_l;
}

std::list<Command *> WaveLevelCommands(Library &lib_p, RandomGenerator &rand_p)
{
	std::list<AreaSpawn> spawners_l;

	Resource res2_l({0,0}, true, lib_p.getEntityModel("resource_food"));
	res2_l._type = ResourceType::Food;
	res2_l._resource = 2000.;
	res2_l._player = 2;

	Resource res3_l({0,0}, true, lib_p.getEntityModel("resource_steel"));
	res3_l._type = ResourceType::Steel;
	res3_l._resource = 2000.;
	res3_l._player = 2;

	Building anchorSpot_l({0,0}, true, lib_p.getBuildingModel("anchor_spot"));
	anchorSpot_l._player = 2;


	{
		AreaSpawn area_l;
		area_l.size = 20;
		area_l.x = 50;
		area_l.y = 0;
		area_l.entities.emplace_back(new Resource(res3_l), 1);
		area_l.entities.emplace_back(new Resource(res2_l), 1);
		spawners_l.push_back(area_l);
	}
	{
		AreaSpawn area_l;
		area_l.size = 10;
		area_l.x = 55;
		area_l.y = 5;
		for(unsigned long c = 0 ; c < 20 ; ++ c)
		{
			Unit *unit_l = new Unit({0, 0}, false, lib_p.getUnitModel(genModelName(rand_p)));
			unit_l->_player = 1;
			area_l.entities.emplace_back(unit_l, 1);
		}
		spawners_l.push_back(area_l);
	}

	std::list<Command *> commands_l {
		new AreaSpawnerCommand(rand_p, spawners_l),
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

WaveSpawn::WaveSpawn(Listener * listener_p, Library const &lib_p, RandomGenerator &rand_p, unsigned long wave_p, unsigned long stepWait_p, unsigned long finalWave_p, unsigned long worldSize_p,
	std::function<std::vector<octopus::Steppable *>(void)> waveStepGenerator_p) :
		OneShotTrigger({listener_p}),
		_lib(lib_p),
		_rand(rand_p),
		_wave(wave_p),
		_stepWait(stepWait_p),
		_finalWave(finalWave_p),
		_worldSize(worldSize_p),
		_waveStepGenerator(waveStepGenerator_p)
{}

void WaveSpawn::trigger(State const &state_p, Step &step_p, unsigned long, octopus::TriggerData const &) const
{
	for(unsigned long i = 0 ; i < _wave * 10 ; ++ i)
	{
		std::string modelName_l = genModelName(_rand);
		Unit unit_l({ _worldSize-_rand.roll(10,20), _worldSize-_rand.roll(10,20) }, false, _lib.getUnitModel(modelName_l));
		unit_l._player = 1;
		Handle handle_l = getNextHandle(step_p, state_p);
		step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));
		step_p.addSteppable(new CommandSpawnStep(new EntityAttackMoveCommand(handle_l, handle_l, {7., 20.}, 0, {{7., 20.}}, true )));
	}
	step_p.addSteppable(new StateRemoveConstraintPositionStep(0, 45, 0, 40, true, true));

	std::vector<octopus::Steppable *> stepsGenerated_l = _waveStepGenerator();
	for(octopus::Steppable *step_l : stepsGenerated_l)
	{
		step_p.addSteppable(step_l);
	}
	step_p.addSteppable(new TriggerSpawn(new WaveSpawn(new ListenerStepCount(_stepWait), _lib, _rand, _wave+1, _stepWait, _finalWave, _worldSize, _waveStepGenerator)));

	// win after X waves
	if(_wave == _finalWave)
	{
		step_p.addSteppable(new StateWinStep(state_p.isOver(), state_p.hasWinningTeam(), state_p.getWinningTeam(), 0));
	}
}

LoseTrigger::LoseTrigger(Listener * listener_p) : OneShotTrigger({listener_p}) {}

void LoseTrigger::trigger(State const &state_p, Step &step_p, unsigned long, octopus::TriggerData const &) const
{
	step_p.addSteppable(new StateWinStep(state_p.isOver(), state_p.hasWinningTeam(), state_p.getWinningTeam(), 1));
}

} // namespace level2
} // namespace godot
