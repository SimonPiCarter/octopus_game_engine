#include "UtilsLevel2.h"

#include "utils/RandomGenerator.hh"
#include "library/Library.hh"
#include "state/entity/Building.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/state/StateAddConstraintPositionStep.hh"

using namespace octopus;

namespace godot
{
namespace level2
{
/// spawners for walls/water

std::list<Steppable *> createWallSpawners(Library &lib_p, unsigned long xStart, unsigned long x, unsigned long y,
	unsigned long startYGate, unsigned long endYGate, Handle &handle_p, std::vector<unsigned long> const &players_p)
{
	std::list<Steppable *> spawners_l = {};

	for(unsigned long player_l : players_p)
	{
		spawners_l.push_back(new StateAddConstraintPositionStep(player_l, x, startYGate, endYGate, true, true));
	}

	// create walls for zone 1
	for(size_t i = 0 ; i <= y ; i+=2)
	{
		if(i > startYGate && i < endYGate)
		{
			continue;
		}
		if(i==startYGate)
		{
			Building building_l({x+1, i+1}, true, lib_p.getBuildingModel("water_v_up"));
			building_l._player = 2;
			spawners_l.push_back(new BuildingSpawnStep(handle_p++, building_l, true));
			continue;
		}
		if(i==endYGate)
		{
			Building building_l({x+1, i+1}, true, lib_p.getBuildingModel("water_v_down"));
			building_l._player = 2;
			spawners_l.push_back(new BuildingSpawnStep(handle_p++, building_l, true));
			continue;
		}
		if(i==y)
		{
			Building building_l({x+1, i+1}, true, lib_p.getBuildingModel("water_corner"));
			building_l._player = 2;
			spawners_l.push_back(new BuildingSpawnStep(handle_p++, building_l, true));
			continue;
		}
		Building building_l({x+1, i+1}, true, lib_p.getBuildingModel("water_v"));
		building_l._player = 2;
		spawners_l.push_back(new BuildingSpawnStep(handle_p++, building_l, true));
	}
	for(size_t i = xStart ; i < x ; i+=2)
	{
		Building building_l({i, y+1}, true, lib_p.getBuildingModel("water_h"));
		building_l._player = 2;
		spawners_l.push_back(new BuildingSpawnStep(handle_p++, building_l, true));
	}

	return spawners_l;
}


} // level2
} // godot
