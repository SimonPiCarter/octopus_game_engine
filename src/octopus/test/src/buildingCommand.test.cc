#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include "command/entity/EntityBuildingCommand.hh"
#include "command/building/BuildingBlueprintCommand.hh"
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
/// This test suite aims at checking that building command works properly
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
	unitModel_l._isBuilder = true;
	Unit unit_l({ 5, 3. }, false, unitModel_l);

	BuildingModel depositModel_l { true, 1., 10. };
	depositModel_l._isBuilding = true;
	depositModel_l._buildingTime = 10;

	Building building_l({1,3}, true, depositModel_l);
	building_l._alive = false;

	UnitSpawnStep * spawn0_l = new UnitSpawnStep(unit_l);
	BuildingSpawnStep * spawn1_l = new BuildingSpawnStep(building_l, false);

	// unit harvest
	EntityBuildingCommand * command_l = new EntityBuildingCommand(0, 0, 1, {1, 3}, 0, {{1, 3}});
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

///
/// Testing that a building is cancelled when the space is taken
/// > 4 entities
///  - unit : 5 3 building
///  - unit : 8 3 building
///  - building : 1, 3
///  - building : 1, 3
///
TEST(buildingCommandTest, cancelled)
{
	//Logger::enable_debug();
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._isBuilder = true;
	Unit unit1_l({ 5, 3. }, false, unitModel_l);
	Unit unit2_l({ 8, 3. }, false, unitModel_l);

	BuildingModel depositModel_l { true, 1., 10. };
	depositModel_l._isBuilding = true;
	depositModel_l._buildingTime = 10;

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new UnitSpawnStep(unit1_l),
		new UnitSpawnStep(unit2_l)
	}, 1.);

	// loop to spawn everything

	// update time to 1 second
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	controller_l.commitCommand(new BuildingBlueprintCommand({1, 3}, 0, depositModel_l));
	controller_l.commitCommand(new BuildingBlueprintCommand({1, 3}, 0, depositModel_l));
	controller_l.commitCommand(new EntityBuildingCommand(0, 0, 2, {1, 3}, 0, {{1, 3}}));
	controller_l.commitCommand(new EntityBuildingCommand(1, 1, 3, {1, 3}, 0, {{1, 3}}));

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(5., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);

	// update time to 3 seconds (4)
	controller_l.update(3.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);
	EXPECT_NEAR(6., state_l->getEntity(1)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(1)->_pos.y, 1e-5);
	Building const * build_l = static_cast<Building const *>(state_l->getEntity(2));
	EXPECT_FALSE(build_l->_alive);

	// update time to 2 seconds (6)
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);
	// entity 1 stopped because blueprint got cancelled
	EXPECT_NEAR(5., state_l->getEntity(1)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(1)->_pos.y, 1e-5);
	build_l = static_cast<Building const *>(state_l->getEntity(2));
	EXPECT_NEAR(2., build_l->_buildingProgress, 1e-5);
	EXPECT_TRUE(build_l->_alive);
}
