#include "MazeLevel.hh"

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

std::list<Steppable *> MazeLevelSteps(Library &lib_p, size_t number_p)
{
	loadModels(lib_p);

	Unit square1_l({ 15, 20. }, false, lib_p.getUnitModel("square"));
	square1_l._player = 0;



	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
	};

	unsigned long id_l = 0;
	for(size_t i = 0; i < number_p ; ++ i)
	{
		spawners_l.push_back(new UnitSpawnStep(id_l++, square1_l));
	}

	Resource res_l({20,20}, true, lib_p.getEntityModel("resource_food"));
	res_l._type = ResourceType::Food;
	res_l._resource = 500.;
	res_l._player = 0;


	std::vector<std::vector<int> > maze_l = {
		{1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,1,0,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
		{1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1},
		{1,0,1,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1},
		{1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,1},
		{1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,0,1},
		{1,0,0,0,1,0,1,0,0,0,1,0,1,0,1,0,1,0,1},
		{1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1},
		{1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1},
		{1,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1},
		{1,1,1,1,1,0,1,0,1,0,1,0,1,1,1,1,1,0,1},
		{1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1},
		{1,0,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1},
		{1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,1,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	};

	for(size_t i = 0 ; i < maze_l.size() ; ++i)
	{
		for(size_t j = 0 ; j < maze_l.at(i).size() ; ++j)
		{
			if(maze_l.at(i).at(j) == 1)
			{
				res_l._pos = Vector(20+2*i, 20+2*j);
				spawners_l.push_back(new ResourceSpawnStep(id_l++, res_l));
			}
		}
	}

	return spawners_l;
}

std::list<Command *> MazeLevelCommands(Library &lib_p)
{
	return {};
}
