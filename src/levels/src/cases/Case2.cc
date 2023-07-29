#include <iostream>
#include <fstream>

#include "command/unit/UnitHarvestCommand.hh"
#include "library/Library.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/entity/Building.hh"
#include "state/entity/Resource.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"

using namespace octopus;

std::list<Steppable *> Case2(Library &lib_p)
{
	UnitModel unitModel_l { false, 1., 0.25, 10. };
	unitModel_l._isUnit = true;
	unitModel_l._maxQuantity["bloc"] = 10;
	lib_p.registerUnitModel("unit", unitModel_l);

	EntityModel resModel_l { true, 3., 1., 10. };
	resModel_l._isResource = true;
	lib_p.registerEntityModel("resource", resModel_l);

	BuildingModel depositModel_l { true, 3., 10. };
	depositModel_l._isBuilding = true;
	depositModel_l._deposit["bloc"] = true;
	lib_p.registerBuildingModel("deposit", depositModel_l);

	Unit unit_l({ 15, 20. }, false, lib_p.getUnitModel("unit"));

	Resource res_l({20,20}, true, lib_p.getEntityModel("resource"));
	res_l._resource = 100.;

	Resource res2_l({21,17}, true, lib_p.getEntityModel("resource"));
	res2_l._resource = 100.;

	Building deposit_l({1, 20}, true, lib_p.getBuildingModel("deposit"));

	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new UnitSpawnStep(Handle(0), unit_l),
		new UnitSpawnStep(Handle(1), unit_l),
		new UnitSpawnStep(Handle(2), unit_l),
		new UnitSpawnStep(Handle(3), unit_l),
		new UnitSpawnStep(Handle(4), unit_l),
		new ResourceSpawnStep(Handle(5), res_l),
		new ResourceSpawnStep(Handle(6), res2_l),
		new BuildingSpawnStep(Handle(7), deposit_l, true),
		new CommandSpawnStep(new UnitHarvestCommand(Handle(0), Handle(0), Handle(5), {20, 20}, 0, {{20, 20}})),
		new CommandSpawnStep(new UnitHarvestCommand(Handle(1), Handle(1), Handle(5), {20, 20}, 0, {{20, 20}})),
		new CommandSpawnStep(new UnitHarvestCommand(Handle(2), Handle(2), Handle(5), {20, 20}, 0, {{20, 20}})),
		new CommandSpawnStep(new UnitHarvestCommand(Handle(3), Handle(3), Handle(5), {20, 20}, 0, {{20, 20}})),
		new CommandSpawnStep(new UnitHarvestCommand(Handle(4), Handle(4), Handle(5), {20, 20}, 0, {{20, 20}}))
	};

	return spawners_l;
}
