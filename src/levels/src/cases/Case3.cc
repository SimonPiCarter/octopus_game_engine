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

using namespace octopus;

std::list<Steppable *> Case3(Library &lib_p)
{
	UnitModel unitModel_l { false, 1., 0.25, 10. };
	unitModel_l._isUnit = true;
	unitModel_l._maxQuantity[ResourceType::Food] = 10;
	unitModel_l._productionTime = 100;
	lib_p.registerUnitModel("unit", unitModel_l);

	BuildingModel buildingModel_l { true, 1., 10. };
	buildingModel_l._isBuilding = true;
	lib_p.registerBuildingModel("building", buildingModel_l);

	Building building_l({1, 20}, true, lib_p.getBuildingModel("building"));
	Unit unit_l({ 15, 20. }, false, lib_p.getUnitModel("unit"));

	// building
	BuildingUnitProductionCommand * command0_l = new BuildingUnitProductionCommand(0, 0, lib_p.getUnitModel("unit"));
	BuildingUnitProductionCommand * command1_l = new BuildingUnitProductionCommand(0, 0, lib_p.getUnitModel("unit"));
	command1_l->setQueued(true);
	BuildingUnitProductionCommand * command2_l = new BuildingUnitProductionCommand(0, 0, lib_p.getUnitModel("unit"));
	command2_l->setQueued(true);
	BuildingUnitProductionCommand * command3_l = new BuildingUnitProductionCommand(0, 0, lib_p.getUnitModel("unit"));
	command3_l->setQueued(true);
	BuildingUnitProductionCommand * command4_l = new BuildingUnitProductionCommand(0, 0, lib_p.getUnitModel("unit"));
	command4_l->setQueued(true);

	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new BuildingSpawnStep(building_l),
		new UnitSpawnStep(unit_l),
		new CommandSpawnStep(command0_l),
		new CommandSpawnStep(command1_l),
		new CommandSpawnStep(command2_l),
		new CommandSpawnStep(command3_l),
		new CommandSpawnStep(command4_l)
	};

	return spawners_l;
}
