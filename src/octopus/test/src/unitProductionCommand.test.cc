#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include "command/BuildingUnitProductionCommand.hh"
#include "controller/Controller.hh"
#include "state/entity/Building.hh"
#include "state/model/building/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/EntityMoveStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"

///
/// This test suite aims at checking that production command works properly
///

using namespace octopus;

///
/// > 1 entity
///  - building : 1, 3 production
///
TEST(unitProductionCommandTest, simple)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._productionTime = 5;


	BuildingModel buildingModel_l { true, 1., 1., 10. };
	buildingModel_l._isBuilding = true;

	Building production_l({1,3}, true, buildingModel_l);

	BuildingSpawnStep * spawn0_l = new BuildingSpawnStep(production_l);

	// unit production
	BuildingUnitProductionCommand * command_l = new BuildingUnitProductionCommand(0, 0, unitModel_l);
	CommandSpawnStep * commandSpawn_l = new CommandSpawnStep(command_l);

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn0_l, commandSpawn_l}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time to 5second (5)
	// at this point production time is over but no spawn yet
	controller_l.update(5.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());

	// update time to 1second (6)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_EQ(1u, state_l->getEntities().size());
}
