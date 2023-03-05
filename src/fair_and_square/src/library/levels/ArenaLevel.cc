#include "ArenaLevel.hh"

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

std::list<Steppable *> ArenaLevelSteps(Library &lib_p)
{
	loadModels(lib_p);

	Unit square1_l({ 15, 20. }, false, lib_p.getUnitModel("square"));
	square1_l._player = 0;
	Unit square2_l({ 35, 20. }, false, lib_p.getUnitModel("square"));
	square2_l._player = 1;

	Unit triangle1_l({ 15, 20. }, false, lib_p.getUnitModel("triangle"));
	triangle1_l._player = 0;
	Unit triangle2_l({ 35, 20. }, false, lib_p.getUnitModel("triangle"));
	triangle2_l._player = 1;

	Unit circle1_l({ 15, 20. }, false, lib_p.getUnitModel("circle"));
	circle1_l._player = 0;
	Unit circle2_l({ 35, 20. }, false, lib_p.getUnitModel("circle"));
	circle2_l._player = 1;

	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new UnitSpawnStep(0, square1_l),
		new UnitSpawnStep(1, square1_l),
		new UnitSpawnStep(2, square1_l),
		new UnitSpawnStep(3, square1_l),
		new UnitSpawnStep(4, square1_l),

		new UnitSpawnStep(5, circle2_l),
		new UnitSpawnStep(6, circle2_l),
		new UnitSpawnStep(7, circle2_l),
		new UnitSpawnStep(8, circle2_l),
		new UnitSpawnStep(9, circle2_l),
	};

	return spawners_l;
}

std::list<Command *> ArenaLevelCommands(Library &lib_p)
{
	return {};
}
