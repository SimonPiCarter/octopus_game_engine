#include "WaveLevel.hh"

#include <iostream>
#include <fstream>
#include <random>

#include "library/model/AnchorTrigger.hh"
#include "library/model/ModelLoader.hh"
#include "library/model/TimerDamage.hh"

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
#include "step/command/flying/FlyingCommandSpawnStep.hh"
#include "step/custom/CustomStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/state/StateWinStep.hh"
#include "step/trigger/TriggerSpawn.hh"

using namespace octopus;

std::vector<octopus::Steppable*> defaultGenerator() { return {}; }

std::string genModelName(std::mt19937 &gen_p)
{
    std::uniform_int_distribution<> dist_l(0, 2);
	std::string model_l = "square";
	int random_l = dist_l(gen_p);
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

std::list<Steppable *> WaveLevelSteps(Library &lib_p, unsigned long waveCount_p, unsigned long stepCount_p, unsigned long player_p, unsigned long worldSize_p,
	std::function<std::vector<octopus::Steppable *>(void)> waveStepGenerator_p)
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
	mapRes_l[octopus::ResourceType::Anchor] = -5;

	Trigger * triggerWave_l = new WaveSpawn(new ListenerStepCount(stepCount_p), lib_p, 1, stepCount_p, waveCount_p, player_p, worldSize_p, waveStepGenerator_p);
	Trigger * triggerLose_l = new LoseTrigger(new ListenerEntityModelDied(&lib_p.getBuildingModel("command_center"), 0));



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
		new TriggerSpawn(new AnchorTrigger(lib_p)),
		new FlyingCommandSpawnStep(new TimerDamage(0, 100, 0, 0, octopus::ResourceType::Anchor, 0)),
	};

	return spawners_l;
}

std::list<Command *> WaveLevelCommands(Library &lib_p, unsigned long worldSize_p)
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

	Building anchorSpot_l({0,0}, true, lib_p.getBuildingModel("anchor_spot"));
	anchorSpot_l._player = 2;

    std::mt19937 gen_l(42);

	int areSize_l = 20;

	for(unsigned long x = 0 ; x < worldSize_p/areSize_l ; ++ x)
	{
		for(unsigned long y = 0 ; y < worldSize_p/areSize_l ; ++ y)
		{
			// skip spawn
			if(x==0 && y==0)
			{
				continue;
			}
			AreaSpawn area_l;
			area_l.size = areSize_l;
			area_l.x = 5 + area_l.size*x;
			area_l.y = 5 + area_l.size*y;
			area_l.entities.emplace_back(new Resource(res3_l), 2);
			area_l.entities.emplace_back(new Resource(res2_l), 3);
			area_l.entities.emplace_back(new Building(anchorSpot_l), 1);
			// only add unit if not on the neighbour of start point
			if(x + y > 1 )
			{
				for(unsigned long c = 0 ; c < 2+x*y ; ++ c)
				{
					Unit *unit_l = new Unit({0, 0}, false, lib_p.getUnitModel(genModelName(gen_l)));
					unit_l->_player = 1;
					area_l.entities.emplace_back(unit_l, 1);
				}
			}
			spawners_l.push_back(area_l);
		}
	}

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

WaveSpawn::WaveSpawn(Listener * listener_p, Library const &lib_p, unsigned long wave_p, unsigned long stepWait_p, unsigned long finalWave_p, unsigned long player_p, unsigned long worldSize_p,
	std::function<std::vector<octopus::Steppable *>(void)> waveStepGenerator_p) :
		OneShotTrigger({listener_p}),
		_lib(lib_p),
		_player(player_p),
		_wave(wave_p),
		_stepWait(stepWait_p),
		_finalWave(finalWave_p),
		_worldSize(worldSize_p),
		_waveStepGenerator(waveStepGenerator_p)
{}

void WaveSpawn::trigger(State const &state_p, Step &step_p, unsigned long) const
{
    std::mt19937 gen_l(42*_wave);
	std::string model_l = genModelName(gen_l);

	// unsigned long n = _stepWait/100/20*_wave;
	// unsigned long nbUnits_l = (10*n*n + 10 * n + 50)/50;
	// unsigned long nLast = 6*_wave -6;
	// unsigned long nbUnitsLast_l = (10*nLast*nLast + 10 * nLast + 50)/50;
	// nbUnits_l = nbUnits_l - nbUnitsLast_l;
	for(unsigned long i = 0 ; i < _wave * 10 ; ++ i)
	{
		Unit unit_l({ _worldSize-10., _worldSize-10. }, false, _lib.getUnitModel(model_l));
		unit_l._player = _player;
		Handle handle_l = getNextHandle(step_p, state_p);
		step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));
		step_p.addSteppable(new CommandSpawnStep(new EntityAttackMoveCommand(handle_l, handle_l, {7., 20.}, 0, {{7., 20.}}, true )));
	}

	std::vector<octopus::Steppable *> stepsGenerated_l = _waveStepGenerator();
	for(octopus::Steppable *step_l : stepsGenerated_l)
	{
		step_p.addSteppable(step_l);
	}
	step_p.addSteppable(new TriggerSpawn(new WaveSpawn(new ListenerStepCount(_stepWait), _lib, _wave+1, _stepWait, _finalWave, _player, _worldSize, _waveStepGenerator)));

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
