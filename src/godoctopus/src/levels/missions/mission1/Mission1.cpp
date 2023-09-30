#include "Mission1.h"

#include <iostream>
#include <fstream>
#include <random>

// fas
// #include "library/model/AnchorTrigger.hh"
#include "library/model/ModelLoader.hh"
// #include "library/model/TimerDamage.hh"

// octopus
#include "controller/trigger/Listener.hh"
#include "controller/trigger/Trigger.hh"
// #include "command/entity/EntityAttackMoveCommand.hh"
// #include "command/spawner/AreaSpawnerCommand.hh"
#include "library/Library.hh"
#include "state/entity/Building.hh"
#include "state/entity/Unit.hh"
#include "state/entity/Resource.hh"
// #include "state/model/entity/BuildingModel.hh"
// #include "state/vision/PatternHandler.hh"
// #include "state/State.hh"
// #include "step/Step.hh"
// #include "step/command/CommandQueueStep.hh"
// #include "step/command/flying/FlyingCommandSpawnStep.hh"
// #include "step/custom/CustomStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/player/PlayerAddBuildingModel.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/state/StateAddConstraintPositionStep.hh"
#include "step/state/StateRemoveConstraintPositionStep.hh"
// #include "step/state/StateTemplePositionAddStep.hh"
// #include "step/state/StateWinStep.hh"
// #include "step/team/TeamVisionStep.hh"
#include "step/trigger/TriggerSpawn.hh"

// godot
#include "controller/step/CameraStep.h"
#include "controller/step/DialogStep.h"

// missions
#include "levels/missions/helpers/DialogTrigger.h"
#include "levels/missions/helpers/SpawnerTrigger.h"
#include "levels/missions/mission1/ZoneTriggers.h"

using namespace octopus;

namespace godot
{
namespace mission
{
void load_from_editor(unsigned long &handle_r, std::list<Steppable *> &spawners_r, octopus::Library const &lib_p, unsigned long player_p);

std::list<Steppable *> Mission1Steps(Library &lib_p, RandomGenerator &rand_p, unsigned long nbPlayers_p)
{
	loadMinimalModels(lib_p);

	unsigned long handle_l = 0;

	std::list<Steppable *> spawners_l;


	std::map<std::string, Fixed> mapRes_l;
	mapRes_l["bloc"] = -200;	mapRes_l["ether"] = -200;

	std::unordered_set<Handle> heroHandles_l;

	for(unsigned long i = 0 ; i < nbPlayers_p ; ++ i)
	{
		spawners_l.push_back(new PlayerSpawnStep(i, 0));
		spawners_l.push_back(new PlayerAddBuildingModel(i, lib_p.getBuildingModel("command_center")));
		spawners_l.push_back(new PlayerAddBuildingModel(i, lib_p.getBuildingModel("barrack_circle")));

		Handle heroHandle_l = Handle(handle_l++);
		heroHandles_l.insert(heroHandle_l);

		Unit unit_l({ 10, 10 }, false, lib_p.getUnitModel("square"));
		unit_l._player = i;
		spawners_l.push_back(new UnitSpawnStep(heroHandle_l, unit_l));

		spawners_l.push_back(new PlayerSpendResourceStep(i, mapRes_l));
		spawners_l.push_back(new godot::CameraStep(to_int(unit_l._pos.x), to_int(unit_l._pos.y), i));

		spawners_l.push_back(new StateAddConstraintPositionStep(i, 26, 36, 56, true, false));
		spawners_l.push_back(new StateAddConstraintPositionStep(i, 48, 35, 57, true, false));

	}

	spawners_l.push_back(new PlayerSpawnStep(nbPlayers_p, 1));
	spawners_l.push_back(new PlayerSpawnStep(nbPlayers_p+1, 2));
	spawners_l.push_back(new godot::DialogStep("mission1_intro"));

	std::vector<Handle> trackers_l;
	std::unordered_set<Handle> trackers_set_l;
	std::unordered_set<Handle> firstGroup_l;
	std::unordered_set<Handle> finalGroup_l;

	// spawn enemies
	for(unsigned long i = 0 ; i < nbPlayers_p ; ++ i)
	{
		// trackers
		{
			Handle enemy_l = Handle(handle_l++);
			trackers_l.push_back(enemy_l);
			trackers_set_l.insert(enemy_l);

			Unit unit_l({ 45, 16 }, false, lib_p.getUnitModel("triangle"));
			unit_l._player = nbPlayers_p;
			spawners_l.push_back(new UnitSpawnStep(enemy_l, unit_l));
		}

		// first enemy group

		for(unsigned long u = 0 ; u < 4  ; ++ u)
		{
			Handle enemy_l = Handle(handle_l++);
			firstGroup_l.insert(enemy_l);

			Unit unit_l({ 47, 35 }, false, lib_p.getUnitModel("circle"));
			unit_l._player = nbPlayers_p;
			spawners_l.push_back(new UnitSpawnStep(enemy_l, unit_l));
		}

		// last group of enemies

		for(unsigned long u = 0 ; u < 7  ; ++ u)
		{
			Handle enemy_l = Handle(handle_l++);
			finalGroup_l.insert(enemy_l);

			Unit unit_l({ 44, 86 }, false, lib_p.getUnitModel("circle"));
			unit_l._player = nbPlayers_p;
			spawners_l.push_back(new UnitSpawnStep(enemy_l, unit_l));
		}
	}

	// First zone triggers (removes obstacle and spawn units) (zone triggers will add the next one in cascade)
	spawners_l.push_back(new TriggerSpawn(
		new RescueTrigger({new ListenerEntityInBox(heroHandles_l, Vector(26,7), Vector(4,11))}, lib_p, heroHandles_l, nbPlayers_p, trackers_l)));

	spawners_l.push_back(new TriggerSpawn(new DialogTrigger({new ListenerEntityDied(trackers_set_l)}, "mission1_trackers_dead")));
	spawners_l.push_back(new TriggerSpawn(new DialogTrigger({new ListenerEntityDied(firstGroup_l)}, "mission1_first_group_dead")));
	spawners_l.push_back(new TriggerSpawn(new DialogTrigger({new ListenerEntityDied(finalGroup_l)}, "mission1_final_group_dead")));

	std::list<Listener*> listeners_l;

	for(unsigned long i = 0 ; i < nbPlayers_p ; ++ i)
	{
		spawners_l.push_back(new TriggerSpawn(new OnEachFunctionTrigger(
			new ListenerEntityModelFinished(&lib_p.getEntityModel("worker"), i),
			[i](State const &, Step &step_p, unsigned long, TriggerData const &)
			{
				std::map<std::string, Fixed> map_l; map_l["worker_count"] = -1;
				step_p.addSteppable(new PlayerSpendResourceStep(i, map_l));
			}
		)));
		spawners_l.push_back(new TriggerSpawn(new OnEachFunctionTrigger(
			new ListenerEntityModelFinished(&lib_p.getEntityModel("circle"), i),
			[i](State const &, Step &step_p, unsigned long, TriggerData const &)
			{
				std::map<std::string, Fixed> map_l; map_l["circle_count"] = -1;
				step_p.addSteppable(new PlayerSpendResourceStep(i, map_l));
			}
		)));
		spawners_l.push_back(new TriggerSpawn(new OnEachFunctionTrigger(
			new ListenerEntityModelFinished(&lib_p.getEntityModel("barrack_circle"), i),
			[i](State const &, Step &step_p, unsigned long, TriggerData const &)
			{
				std::map<std::string, Fixed> map_l; map_l["barrack_circle"] = -1;
				step_p.addSteppable(new PlayerSpendResourceStep(i, map_l));
			}
		)));

		listeners_l.push_back(new ListenerResource<true>(i, "worker_count", 5));
		listeners_l.push_back(new ListenerResource<true>(i, "barrack_circle", 1));
		listeners_l.push_back(new ListenerResource<true>(i, "circle_count", 5));
	}

	for(unsigned long i = 0 ; i < nbPlayers_p ; ++ i)
	{
		spawners_l.push_back(new TriggerSpawn(new OneShotFunctionTrigger(
			{listeners_l},
			[i](State const &, Step &step_p, unsigned long, TriggerData const &)
			{
				step_p.addSteppable(new StateRemoveConstraintPositionStep(i, 48, 35, 57, true, false));
				step_p.addSteppable(new godot::DialogStep("mission1_base_building_done"));
			}
		)));
	}

	load_from_editor(handle_l, spawners_l, lib_p, nbPlayers_p+1);


	// spawn resources
	for(unsigned long i = 0 ; i < nbPlayers_p ; ++ i)
	{
		Resource res1_l({87,34+3*i-1}, true, lib_p.getResourceModel("resource_bloc"));
		res1_l._resource = 10000;
		res1_l._player = nbPlayers_p+1;
		Resource res2_l({87,34+3*i+1}, true, lib_p.getResourceModel("resource_ether"));
		res2_l._resource = 10000;
		res2_l._player = nbPlayers_p+1;

		spawners_l.push_back(new ResourceSpawnStep(Handle(handle_l++), res1_l));
		spawners_l.push_back(new ResourceSpawnStep(Handle(handle_l++), res2_l));
	}

	return spawners_l;
}

std::list<Command *> Mission1Commands(Library &lib_p, RandomGenerator &rand_p, unsigned long nbPlayers_p)
{
	std::list<Command *> commands_l {
	};

	return commands_l;
}

/// @brief write header for classic arena level
void writeMission1Header(std::ofstream &file_p, Mission1Header const &header_p)
{
    file_p.write((char*)&header_p.seed, sizeof(header_p.seed));
    file_p.write((char*)&header_p.nb_players, sizeof(header_p.nb_players));
}

/// @brief read header for classic arena level and return a pair of steppable and command
std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > readWaveLevelHeader(octopus::Library &lib_p, std::ifstream &file_p,
	octopus::RandomGenerator * &rand_p, Mission1Header &header_r)
{
    file_p.read((char*)&header_r.seed, sizeof(header_r.seed));
    file_p.read((char*)&header_r.nb_players, sizeof(header_r.nb_players));

	delete rand_p;
	rand_p = new octopus::RandomGenerator(header_r.seed);

	std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > pair_l;
	pair_l.first = Mission1Steps(lib_p, *rand_p, header_r.nb_players);
	pair_l.second = Mission1Commands(lib_p, *rand_p, header_r.nb_players);
	return pair_l;
}

void spawn_from_editor(unsigned long &handle_r, std::list<Steppable *> &spawners_r, unsigned long player_p, octopus::Library const &lib_p, int x, int y, int index)
{
	if(index == 3)
	{
		Building tree_l({x,y}, true, lib_p.getBuildingModel("water_v_up"));
		tree_l._player = player_p;
		spawners_r.push_back(new BuildingSpawnStep(Handle(handle_r++), tree_l, true));
	}
}

void load_from_editor(unsigned long &handle_r, std::list<Steppable *> &spawners_r, octopus::Library const &lib_p, unsigned long player_p)
{
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 0,4,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 0,17,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 0,19,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 1,8,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 2,3,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 2,5,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 2,19,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 3,15,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 3,21,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 3,87,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 4,2,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 4,5,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 4,19,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 4,43,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 4,84,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 5,77,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 5,95,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 6,3,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 6,18,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 6,28,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 7,5,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 7,20,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 7,37,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 7,50,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 8,18,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 8,26,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 8,30,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 8,65,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 8,91,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 8,98,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 9,0,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 9,3,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 9,5,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 9,34,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 10,19,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 10,23,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 10,85,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 11,3,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 11,17,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 12,5,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 12,46,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 12,72,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 12,94,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 13,1,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 13,17,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 13,61,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 14,3,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 14,19,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 14,27,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 14,33,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 14,53,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 14,92,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 15,5,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 15,7,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 15,24,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 16,18,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 16,39,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 16,78,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 16,103,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 17,7,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 17,20,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 17,24,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 17,30,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 17,44,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 17,76,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 17,97,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 18,5,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 18,63,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 18,90,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 19,19,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 19,21,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 19,39,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 19,70,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 19,85,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 20,5,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 20,68,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 20,111,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 21,7,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 21,19,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 21,30,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 21,34,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 21,97,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 21,108,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 22,4,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 22,22,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 22,26,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 22,47,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 22,77,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 22,88,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 22,91,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 22,95,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 23,6,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 23,17,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 23,19,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 23,59,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 23,67,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 23,101,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 24,41,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 24,78,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 24,81,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 24,83,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 24,87,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 24,95,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 24,97,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 24,104,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 25,5,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 25,18,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 25,22,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 25,51,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 25,69,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 25,71,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 25,75,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 25,85,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 25,92,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 25,100,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 26,3,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 26,7,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 26,30,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 26,34,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 26,73,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 26,77,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 26,81,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 26,87,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 26,89,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 26,94,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 26,98,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 27,19,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 27,24,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 27,66,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 27,68,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 27,71,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 27,79,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 27,85,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 27,91,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 28,4,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 28,21,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 28,89,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 28,95,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 28,101,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 29,25,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 29,40,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 29,52,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 29,56,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 29,61,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 29,66,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 29,68,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 29,97,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 29,99,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 29,103,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 30,3,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 30,19,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 30,21,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 30,23,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 30,47,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 30,54,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 30,59,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 30,64,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 31,51,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 31,56,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 31,62,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 31,99,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 31,102,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 32,4,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 32,19,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 32,25,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 32,28,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 32,31,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 32,33,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 32,54,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 32,58,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 32,112,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 33,6,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 33,21,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 33,45,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 33,47,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 33,51,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 33,100,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 33,104,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 34,8,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 34,24,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 34,28,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 34,30,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 34,33,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 34,35,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 34,40,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 34,42,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 34,49,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 34,102,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 35,6,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 35,22,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 35,26,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 35,37,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 35,44,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 35,47,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 36,28,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 36,30,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 36,33,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 36,40,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 36,103,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 36,105,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 36,108,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 37,6,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 37,8,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 37,38,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 38,30,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 38,105,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 39,8,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 40,104,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 41,8,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 41,10,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 41,106,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 43,9,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 43,106,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 44,81,1);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 44,104,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 44,112,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 45,7,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 45,9,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 46,11,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 46,104,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 46,106,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 47,37,1);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 48,8,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 48,11,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 48,17,1);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 48,106,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 49,104,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 50,11,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 51,8,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 51,105,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 52,12,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 52,60,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 53,10,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 53,58,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 53,105,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 54,7,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 54,61,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 54,63,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 54,65,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 54,68,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 54,103,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 54,107,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 55,10,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 55,53,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 55,57,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 55,70,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 56,22,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 56,26,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 56,28,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 56,30,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 56,55,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 56,62,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 56,65,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 56,67,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 56,72,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 56,105,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 57,8,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 57,11,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 57,24,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 57,46,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 57,50,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 57,52,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 57,57,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 57,60,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 57,70,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 57,74,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 57,103,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 57,112,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 58,22,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 58,27,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 58,32,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 58,48,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 58,55,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 59,8,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 59,10,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 59,24,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 59,30,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 59,41,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 59,44,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 59,46,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 59,53,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 59,72,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 59,75,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 59,100,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 59,103,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 60,33,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 61,9,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 61,11,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 61,20,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 61,24,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 61,26,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 61,44,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 61,73,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 61,75,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 61,101,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 61,103,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 61,112,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 62,31,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 62,77,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 62,79,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 63,11,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 63,21,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 63,23,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 63,43,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 63,71,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 63,81,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 63,83,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 63,89,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 63,91,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 63,103,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 64,9,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 64,57,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 64,59,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 64,62,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 64,85,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 64,87,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 64,101,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 65,11,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 65,21,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 65,25,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 65,44,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 65,92,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 65,96,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 65,98,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 66,9,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 66,46,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 66,48,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 66,66,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 66,78,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 66,89,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 66,94,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 66,101,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 67,12,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 67,22,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 67,25,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 67,81,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 67,83,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 67,86,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 67,99,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 67,108,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 68,8,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 68,46,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 68,90,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 68,92,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 68,94,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 69,11,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 69,21,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 69,24,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 69,48,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 69,55,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 69,69,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 69,77,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 69,84,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 69,88,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 69,98,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 70,6,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 70,50,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 70,58,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 70,65,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 70,96,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 71,9,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 71,12,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 71,22,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 71,26,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 71,87,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 71,92,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 71,103,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 72,24,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 72,50,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 72,52,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 72,81,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 73,9,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 73,11,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 73,26,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 73,71,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 73,85,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 74,53,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 74,95,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 75,10,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 75,23,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 75,27,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 75,76,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 76,7,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 76,29,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 76,54,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 76,81,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 76,110,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 77,10,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 77,56,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 77,63,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 77,67,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 77,96,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 77,100,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 78,12,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 78,27,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 78,29,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 78,52,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 78,59,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 79,6,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 79,8,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 79,10,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 79,55,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 79,63,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 79,92,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 79,107,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 80,28,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 80,79,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 80,89,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 81,10,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 81,12,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 81,18,1);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 81,26,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 81,56,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 81,62,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 81,71,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 82,8,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 82,29,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 82,41,1);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 82,54,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 82,58,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 83,12,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 83,31,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 83,84,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 84,10,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 84,27,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 84,54,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 84,57,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 85,8,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 85,29,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 85,79,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 85,101,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 86,10,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 86,12,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 86,26,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 86,56,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 86,69,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 87,54,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 87,64,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 87,107,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 88,9,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 88,13,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 88,26,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 88,28,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 88,75,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 88,92,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 89,15,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 89,55,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 89,58,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 90,8,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 90,11,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 90,24,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 90,26,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 90,28,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 91,14,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 91,18,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 91,22,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 91,54,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 92,9,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 92,16,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 92,25,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 92,29,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 92,56,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 92,69,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 92,82,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 93,11,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 93,14,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 93,19,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 93,27,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 93,98,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 94,21,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 94,23,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 94,25,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 94,30,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 94,32,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 94,42,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 94,54,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 94,56,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 94,58,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 94,61,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 94,90,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 95,11,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 95,15,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 95,17,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 95,36,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 95,40,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 95,44,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 96,24,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 96,29,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 96,32,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 96,34,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 96,38,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 96,52,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 96,54,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 96,79,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 97,9,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 97,13,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 97,16,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 97,22,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 97,27,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 97,40,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 97,42,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 97,45,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 97,47,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 98,18,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 98,20,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 98,24,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 98,32,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 98,49,3);
spawn_from_editor(handle_r, spawners_r, player_p, lib_p, 98,51,3);
}

} // namespace mission1
} // namespace godot
