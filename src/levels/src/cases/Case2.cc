#include <iostream>
#include <fstream>

#include "command/EntityMoveCommand.hh"
#include "command/EntityWaitCommand.hh"
#include "command/EntityAttackCommand.hh"
#include "command/UnitHarvestCommand.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/entity/Building.hh"
#include "state/entity/Resource.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/model/building/BuildingModel.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/EntitySpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"

using namespace octopus;

std::list<octopus::Steppable *> Case2()
{
	UnitModel unitModel_l { false, 1., 0.25, 10. };
	unitModel_l._isUnit = true;
	unitModel_l._maxQuantity[ResourceType::Food] = 10;
	Unit unit_l({ 15, 20. }, false, unitModel_l);

	EntityModel resModel_l { true, 3., 1., 10. };
	resModel_l._isResource = true;
	Resource res_l({20,20}, true, resModel_l);
	res_l._resource = 100.;
	Resource res2_l({21,17}, true, resModel_l);
	res2_l._resource = 100.;

	BuildingModel depositModel_l { true, 3., 1., 10. };
	depositModel_l._isBuilding = true;
	depositModel_l._deposit[ResourceType::Food] = true;

	Building deposit_l({1, 20}, true, depositModel_l);

	// unit harvest
	UnitHarvestCommand * command0_l = new UnitHarvestCommand(0, 0, 5, {20, 20}, 0, {{20, 20}});
	UnitHarvestCommand * command1_l = new UnitHarvestCommand(1, 1, 5, {20, 20}, 0, {{20, 20}});
	UnitHarvestCommand * command2_l = new UnitHarvestCommand(2, 2, 5, {20, 20}, 0, {{20, 20}});
	UnitHarvestCommand * command3_l = new UnitHarvestCommand(3, 3, 5, {20, 20}, 0, {{20, 20}});
	UnitHarvestCommand * command4_l = new UnitHarvestCommand(4, 4, 5, {20, 20}, 0, {{20, 20}});

	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new UnitSpawnStep(unit_l),
		new UnitSpawnStep(unit_l),
		new UnitSpawnStep(unit_l),
		new UnitSpawnStep(unit_l),
		new UnitSpawnStep(unit_l),
		new ResourceSpawnStep(res_l),
		new ResourceSpawnStep(res2_l),
		new BuildingSpawnStep(deposit_l),
		new CommandSpawnStep(command0_l),
		new CommandSpawnStep(command1_l),
		new CommandSpawnStep(command2_l),
		new CommandSpawnStep(command3_l),
		new CommandSpawnStep(command4_l)
	};

	return spawners_l;
}
