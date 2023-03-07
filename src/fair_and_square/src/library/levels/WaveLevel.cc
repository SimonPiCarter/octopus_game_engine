#include "WaveLevel.hh"

#include <iostream>
#include <fstream>
#include <random>

#include "library/model/ModelLoader.hh"

#include "controller/trigger/Listener.hh"
#include "command/entity/EntityAttackMoveCommand.hh"
#include "command/spawner/AreaSpawnerCommand.hh"
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

std::list<Steppable *> WaveLevelSteps(Library &lib_p, unsigned long waveCount_p, unsigned long stepCount_p, unsigned long player_p)
{
	loadModels(lib_p);

	Building building_l({4, 20}, true, lib_p.getBuildingModel("command_center"));
	Unit unit_l({ 15, 20. }, false, lib_p.getUnitModel("worker"));

	Resource res1_l({20,20}, true, lib_p.getEntityModel("resource_food"));
	res1_l._type = ResourceType::Food;
	res1_l._resource = 500.;
	res1_l._player = 2;

	Resource res2_l({21,17}, true, lib_p.getEntityModel("resource_food"));
	res2_l._type = ResourceType::Food;
	res2_l._resource = 500.;
	res2_l._player = 2;

	Resource res3_l({15,17}, true, lib_p.getEntityModel("resource_steel"));
	res3_l._type = ResourceType::Steel;
	res3_l._resource = 500.;
	res3_l._player = 2;

	std::map<ResourceType, double> mapRes_l;
	mapRes_l[octopus::ResourceType::Food] = -200;
	mapRes_l[octopus::ResourceType::Steel] = -200;

	Trigger * triggerWave_l = new WaveSpawn(new ListenerStepCount(stepCount_p), lib_p, 1, stepCount_p, waveCount_p, player_p);
	Trigger * triggerLose_l = new LoseTrigger(new ListenerEntityModelDied(&lib_p.getBuildingModel("command_center"), 0));

	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new PlayerSpawnStep(2, 2),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("command_center")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("barrack")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("deposit")),
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

std::list<Command *> WaveLevelCommands(Library &lib_p)
{
	std::list<AreaSpawn> spawners_l;

	Resource res2_l({21,17}, true, lib_p.getEntityModel("resource_food"));
	res2_l._type = ResourceType::Food;
	res2_l._resource = 500.;
	res2_l._player = 2;

	Resource res3_l({15,17}, true, lib_p.getEntityModel("resource_steel"));
	res3_l._type = ResourceType::Steel;
	res3_l._resource = 500.;
	res3_l._player = 2;

	AreaSpawn area_l;
	area_l.x = 25;
	area_l.y = 5;
	area_l.size = 20;
	area_l.entities.emplace_back(new Resource(res3_l), 1);
	area_l.entities.emplace_back(new Resource(res2_l), 3);
	spawners_l.push_back(area_l);

	area_l.x = 5;
	area_l.y = 25;
	area_l.size = 20;
	area_l.entities.clear();
	area_l.entities.emplace_back(new Resource(res3_l), 1);
	area_l.entities.emplace_back(new Resource(res2_l), 3);
	spawners_l.push_back(area_l);

	AreaSpawnerCommand * spawnCommand_l = new AreaSpawnerCommand(spawners_l);

	std::list<Command *> commands_l {
		new AreaSpawnerCommand(spawners_l),
	};

	return commands_l;
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/// 				Triggers			  ///
/////////////////////////////////////////////
/////////////////////////////////////////////

WaveSpawn::WaveSpawn(Listener * listener_p, Library const &lib_p, unsigned long wave_p, unsigned long stepWait_p, unsigned long finalWave_p, unsigned long player_p) :
		OneShotTrigger({listener_p}),
		_lib(lib_p),
		_player(player_p),
		_wave(wave_p),
		_stepWait(stepWait_p),
		_finalWave(finalWave_p)
{}

void WaveSpawn::trigger(State const &state_p, Step &step_p, unsigned long) const
{
	for(unsigned long i = 0 ; i < _wave * 10 ; ++ i)
	{
		Unit unit_l({ 35., 35. }, false, _lib.getUnitModel("square"));
		unit_l._player = _player;
		Handle handle_l = getNextHandle(step_p, state_p);
		step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));
		step_p.addSteppable(new CommandSpawnStep(new EntityAttackMoveCommand(handle_l, handle_l, {7., 20.}, 0, {{7., 20.}}, true )));
	}

	step_p.addSteppable(new TriggerSpawn(new WaveSpawn(new ListenerStepCount(_stepWait), _lib, _wave+1, _stepWait, _finalWave, _player)));

	// win after 10 waves
	if(_wave == _finalWave)
	{
		step_p.addSteppable(new StateWinStep(state_p.isOver(), state_p.hasWinningTeam(), state_p.getWinningTeam(), 0));
	}
}

LoseTrigger::LoseTrigger(Listener * listener_p) : OneShotTrigger({listener_p}) {}

void LoseTrigger::trigger(State const &state_p, Step &step_p, unsigned long) const
{
	step_p.addSteppable(new StateWinStep(state_p.isOver(), state_p.hasWinningTeam(), state_p.getWinningTeam(), 1));
}
