#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include "command/BuildingCommand.hh"
#include "controller/Controller.hh"
#include "state/entity/Building.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"

///
/// This test suite aims at checking that Harvest command works properly
///

using namespace octopus;

///
/// > 2 entities
///  - unit : 5 3 building
///  - building : 1, 3
///
TEST(buildingCommandTest, simple)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	Unit unit_l({ 5, 3. }, false, unitModel_l);

	BuildingModel depositModel_l { true, 1., 10. };
	depositModel_l._isBuilding = true;
	depositModel_l._buildingTime = 10;

	Building building_l({1,3}, true, depositModel_l);
	building_l._alive = false;

	UnitSpawnStep * spawn0_l = new UnitSpawnStep(unit_l);
	BuildingSpawnStep * spawn1_l = new BuildingSpawnStep(building_l);

	// unit harvest
	BuildingCommand * command_l = new BuildingCommand(0, 0, 1, {1, 3}, 0, {{1, 3}});
	CommandSpawnStep * commandSpawn_l = new CommandSpawnStep(command_l);

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn0_l, spawn1_l, commandSpawn_l}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(5., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);

	// update time to 2 seconds (2)
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);
	Building const * build_l = static_cast<Building const *>(state_l->getEntity(1));
	EXPECT_FALSE(build_l->_alive);

	// update time to 2 seconds (4)
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);
	build_l = static_cast<Building const *>(state_l->getEntity(1));
	EXPECT_NEAR(2., build_l->_buildingProgress, 1e-5);
	EXPECT_TRUE(build_l->_alive);
}
