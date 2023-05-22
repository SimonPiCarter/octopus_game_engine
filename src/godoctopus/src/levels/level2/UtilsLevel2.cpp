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

std::string genModelName(octopus::RandomGenerator &gen_p)
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

/// spawners for walls/water

std::list<Steppable *> createWallSpawners(Library &lib_p, unsigned long x, unsigned long y, unsigned long startYGate, unsigned long endYGate, Handle &handle_p)
{
	std::list<Steppable *> spawners_l = {
		new StateAddConstraintPositionStep(0, x, 0, y, true, true),
		new StateAddConstraintPositionStep(0, y, 0, x, true, false),
	};

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
	for(size_t i = 0 ; i < x ; i+=2)
	{
		Building building_l({i, y+1}, true, lib_p.getBuildingModel("water_h"));
		building_l._player = 2;
		spawners_l.push_back(new BuildingSpawnStep(handle_p++, building_l, true));
	}

	return spawners_l;
}


} // level2
} // godot
