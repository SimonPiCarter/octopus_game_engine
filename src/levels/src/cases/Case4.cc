#include <iostream>
#include <fstream>

#include "command/building/BuildingUnitProductionCommand.hh"
#include "library/Library.hh"
#include "state/entity/Building.hh"
#include "state/entity/Unit.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"

using namespace octopus;

std::list<Steppable *> Case4(Library &lib_p)
{
	UnitModel unitModel_l { false, 1., 0.05, 10. };
	unitModel_l._isUnit = true;
	unitModel_l._maxQuantity[ResourceType::Food] = 10;
	unitModel_l._productionTime = 100;
	unitModel_l._cost[ResourceType::Food] = 50;
	lib_p.registerUnitModel("unit", unitModel_l);

	BuildingModel buildingModel_l { true, 1., 10. };
	buildingModel_l._isBuilding = true;
	lib_p.registerBuildingModel("building", buildingModel_l);

	Building building_l({1, 20}, true, lib_p.getBuildingModel("building"));
	Unit unit_l({ 15, 20. }, false, lib_p.getUnitModel("unit"));

	std::map<ResourceType, double> mapRes_l;
	mapRes_l[octopus::ResourceType::Food] = -200;

	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpendResourceStep(0, mapRes_l),
		new BuildingSpawnStep(building_l),
		new UnitSpawnStep(unit_l)
	};

	return spawners_l;
}
