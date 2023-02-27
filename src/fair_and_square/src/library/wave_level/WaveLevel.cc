#include <iostream>
#include <fstream>
#include <random>

#include "ModelLoader.hh"

#include "controller/trigger/Trigger.hh"
#include "controller/trigger/Listener.hh"
#include "command/entity/EntityAttackMoveCommand.hh"
#include "library/Library.hh"
#include "state/entity/Building.hh"
#include "state/entity/Unit.hh"
#include "state/entity/Resource.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/state/StateWinStep.hh"
#include "step/trigger/TriggerSpawn.hh"

using namespace octopus;

class WaveSpawn : public OneShotTrigger
{
public:
	WaveSpawn(Listener * listener_p, Library const &lib_p, unsigned long wave_p) : OneShotTrigger({listener_p}), _lib(lib_p), _wave(wave_p) {}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long) const override
	{
		for(unsigned long i = 0 ; i < _wave * 10 ; ++ i)
		{
			Unit unit_l({ 35., 35. }, false, _lib.getUnitModel("square"));
			unit_l._player = 1;
			Handle handle_l = getNextHandle(step_p, state_p);
			step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));
			step_p.addSteppable(new CommandSpawnStep(new EntityAttackMoveCommand(handle_l, handle_l, {7., 20.}, 0, {{7., 20.}}, true )));
		}

		step_p.addSteppable(new TriggerSpawn(new WaveSpawn(new ListenerStepCount(3*60*100), _lib, _wave+1)));

		// win after 10 waves
		if(_wave == 10)
		{
			step_p.addSteppable(new StateWinStep(state_p.isOver(), state_p.hasWinningTeam(), state_p.getWinningTeam(), 0));
		}
	}

private:
	Library const &_lib;
	unsigned long const _wave;
};

class LoseTrigger : public OneShotTrigger
{
public:
	LoseTrigger(Listener * listener_p) : OneShotTrigger({listener_p}) {}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long) const override
	{
		step_p.addSteppable(new StateWinStep(state_p.isOver(), state_p.hasWinningTeam(), state_p.getWinningTeam(), 1));
	}
};

std::list<Steppable *> WaveLevel(Library &lib_p)
{
	loadModels(lib_p);

	Building building_l({4, 20}, true, lib_p.getBuildingModel("command_center"));
	Unit unit_l({ 15, 20. }, false, lib_p.getUnitModel("worker"));

	Resource res1_l({20,20}, true, lib_p.getEntityModel("resource_food"));
	res1_l._type = ResourceType::Food;
	res1_l._resource = 500.;

	Resource res2_l({21,17}, true, lib_p.getEntityModel("resource_food"));
	res2_l._type = ResourceType::Food;
	res2_l._resource = 500.;

	Resource res3_l({15,17}, true, lib_p.getEntityModel("resource_steel"));
	res3_l._type = ResourceType::Steel;
	res3_l._resource = 500.;

	std::map<ResourceType, double> mapRes_l;
	mapRes_l[octopus::ResourceType::Food] = -200;
	mapRes_l[octopus::ResourceType::Steel] = -200;

	Trigger * triggerWave_l = new WaveSpawn(new ListenerStepCount(3*60*100), lib_p, 1);
	Trigger * triggerLose_l = new LoseTrigger(new ListenerEntityModelDied(&lib_p.getBuildingModel("command_center"), 0));

	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("command_center")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("barrack")),
		new PlayerSpendResourceStep(0, mapRes_l),
		new BuildingSpawnStep(0, building_l, true),
		new ResourceSpawnStep(1, res1_l),
		new ResourceSpawnStep(2, res2_l),
		new ResourceSpawnStep(3, res3_l),
		new UnitSpawnStep(4, unit_l),
		new UnitSpawnStep(5, unit_l),
		new UnitSpawnStep(6, unit_l),
		new UnitSpawnStep(7, unit_l),
		new UnitSpawnStep(8, unit_l),
		new TriggerSpawn(triggerWave_l),
		new TriggerSpawn(triggerLose_l),
	};

	return spawners_l;
}
