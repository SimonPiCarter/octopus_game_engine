#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include "command/unit/UnitHarvestCommand.hh"
#include "controller/Controller.hh"
#include "state/entity/Building.hh"
#include "state/entity/Resource.hh"
#include "state/model/entity/BuildingModel.hh"
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
/// This test suite aims at checking that Harvest command works properly
///

using namespace octopus;

///
/// > 3 entities
///  - unit : 5 3 harvesting
///  - building : 1, 3 deposit for Food
///  - resource : 9, 3 resource of food (20 food)
///
TEST(harvestCommandTest, simple)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._maxQuantity[ResourceType::Food] = 10;
	Unit unit_l({ 5.5, 3.5 }, false, unitModel_l);

	EntityModel resModel_l { true, 1., 1., 10. };
	resModel_l._isResource = true;
	Resource res_l({9.5,3.5}, true, resModel_l);
	res_l._resource = 20.;

	BuildingModel depositModel_l { true, 1., 10. };
	depositModel_l._isBuilding = true;
	depositModel_l._deposit[ResourceType::Food] = true;

	Building deposit_l({1.5,3.5}, true, depositModel_l);

	UnitSpawnStep * spawn0_l = new UnitSpawnStep(0, unit_l);
	ResourceSpawnStep * spawn1_l = new ResourceSpawnStep(1, res_l);
	BuildingSpawnStep * spawn2_l = new BuildingSpawnStep(2, deposit_l, true);

	// unit harvest
	UnitHarvestCommand * command_l = new UnitHarvestCommand(0, 0, 1, {9.5, 3.5}, 0, {{9.5, 3.5}});
	CommandSpawnStep * commandSpawn_l = new CommandSpawnStep(command_l);

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn0_l, spawn1_l, spawn2_l, commandSpawn_l}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(5.5, state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.y, 1e-5);

	// update time to 1second (1)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(6.5, state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.y, 1e-5);

	// update time to 1 seconds (2)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(7.5, state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.y, 1e-5);

	// update time to 1 seconds (3)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	Unit const * stateUnit_l = static_cast<Unit const *>(state_l->getEntity(0));

	EXPECT_NEAR(7.5, state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.y, 1e-5);
	EXPECT_EQ(ResourceType::Food, stateUnit_l->_typeOfResource);
	EXPECT_NEAR(1., stateUnit_l->_quantityOfResource, 1e-5);

	// update time to 9 seconds (12)
	controller_l.update(9.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(0));

	EXPECT_NEAR(7.5, state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.y, 1e-5);
	EXPECT_EQ(ResourceType::Food, stateUnit_l->_typeOfResource);
	EXPECT_NEAR(10., stateUnit_l->_quantityOfResource, 1e-5);

	// update time to 2 seconds (14) : scanning for deposit + moving
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(0));

	EXPECT_NEAR(6.5, state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.y, 1e-5);
	EXPECT_EQ(ResourceType::Food, stateUnit_l->_typeOfResource);
	EXPECT_NEAR(10., stateUnit_l->_quantityOfResource, 1e-5);

	// update time to 3 seconds (17)
	controller_l.update(3.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(0));

	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.y, 1e-5);
	EXPECT_EQ(ResourceType::Food, stateUnit_l->_typeOfResource);
	EXPECT_NEAR(10., stateUnit_l->_quantityOfResource, 1e-5);

	// update time to 1 seconds (18)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(0));
	Player const * player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.y, 1e-5);
	EXPECT_EQ(ResourceType::Food, stateUnit_l->_typeOfResource);
	EXPECT_NEAR(0., stateUnit_l->_quantityOfResource, 1e-5);
	EXPECT_NEAR(10., player_l->_resources.at(ResourceType::Food), 1e-5);

	// update time to 19 seconds (27) : full trip just before trop
	// 4 steps : move to harvest
	// 10 steps : harvest 10
	// 1 step : scan for depoit
	// 4 steps : move to deposit
	controller_l.update(19.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(0));
	player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.y, 1e-5);
	EXPECT_EQ(ResourceType::Food, stateUnit_l->_typeOfResource);
	EXPECT_NEAR(10., stateUnit_l->_quantityOfResource, 1e-5);
	EXPECT_NEAR(10., player_l->_resources.at(ResourceType::Food), 1e-5);

	// update time to 1 seconds (28) : drop
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(0));
	player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.y, 1e-5);
	EXPECT_EQ(ResourceType::Food, stateUnit_l->_typeOfResource);
	EXPECT_NEAR(0., stateUnit_l->_quantityOfResource, 1e-5);
	EXPECT_NEAR(20., player_l->_resources.at(ResourceType::Food), 1e-5);

	// no more resurce to harvest stop here

	// update time to 10 seconds (38) : drop
	controller_l.update(10.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(0));
	player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.y, 1e-5);
	EXPECT_EQ(ResourceType::Food, stateUnit_l->_typeOfResource);
	EXPECT_NEAR(0., stateUnit_l->_quantityOfResource, 1e-5);
	EXPECT_NEAR(20., player_l->_resources.at(ResourceType::Food), 1e-5);
}

///
/// > 4 entities
///  - unit : 5 3 harvesting
///  - building : 1, 3 deposit for Food
///  - resource : 9, 3 resource of food (10 food)
///  - resource : 10, 3 resource of food (10 food)
///
TEST(harvestCommandTest, simple_new_res)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._maxQuantity[ResourceType::Food] = 10;
	Unit unit_l({ 5.5, 3.5 }, false, unitModel_l);

	EntityModel resModel_l { true, 1., 1., 10. };
	resModel_l._isResource = true;
	Resource res_l({9.5,3.5}, true, resModel_l);
	res_l._resource = 10.;
	Resource res2_l({10.5,3.5}, true, resModel_l);
	res2_l._resource = 10.;

	BuildingModel depositModel_l { true, 1., 10. };
	depositModel_l._isBuilding = true;
	depositModel_l._deposit[ResourceType::Food] = true;

	Building deposit_l({1.5,3.5}, true, depositModel_l);

	std::list<Steppable *> spawners_l;
	spawners_l.push_back(new PlayerSpawnStep(0, 0));
	spawners_l.push_back(new UnitSpawnStep(0, unit_l));
	spawners_l.push_back(new ResourceSpawnStep(1, res_l));
	spawners_l.push_back(new ResourceSpawnStep(2, res2_l));
	spawners_l.push_back(new BuildingSpawnStep(3, deposit_l, true));

	// unit harvest
	UnitHarvestCommand * command_l = new UnitHarvestCommand(0, 0, 1, {9.5, 3.5}, 0, {{9.5, 3.5}});
	spawners_l.push_back(new CommandSpawnStep(command_l));

	Controller controller_l(spawners_l, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	// update time to 1 seconds (18)
	controller_l.update(18.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	Unit const * stateUnit_l = static_cast<Unit const *>(state_l->getEntity(0));
	Player const * player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.y, 1e-5);
	EXPECT_EQ(ResourceType::Food, stateUnit_l->_typeOfResource);
	EXPECT_NEAR(0., stateUnit_l->_quantityOfResource, 1e-5);
	EXPECT_NEAR(10., player_l->_resources.at(ResourceType::Food), 1e-5);

	// update time to 22 seconds (30) : full trip just before trop
	// 1 step : scan for new resource
	// 5 steps : move to harvest
	// 10 steps : harvest 10
	// 1 step : scan for depoit
	// 5 steps : move to deposit
	controller_l.update(22.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(0));
	player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.y, 1e-5);
	EXPECT_EQ(ResourceType::Food, stateUnit_l->_typeOfResource);
	EXPECT_NEAR(10., stateUnit_l->_quantityOfResource, 1e-5);
	EXPECT_NEAR(10., player_l->_resources.at(ResourceType::Food), 1e-5);

	// update time to 1 seconds (31) : drop
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(0));
	player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.y, 1e-5);
	EXPECT_EQ(ResourceType::Food, stateUnit_l->_typeOfResource);
	EXPECT_NEAR(0., stateUnit_l->_quantityOfResource, 1e-5);
	EXPECT_NEAR(20., player_l->_resources.at(ResourceType::Food), 1e-5);

	// no more resurce to harvest stop here

	// update time to 10 seconds (41) : drop
	controller_l.update(10.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(0));
	player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3.5, state_l->getEntity(0)->_pos.y, 1e-5);
	EXPECT_EQ(ResourceType::Food, stateUnit_l->_typeOfResource);
	EXPECT_NEAR(0., stateUnit_l->_quantityOfResource, 1e-5);
	EXPECT_NEAR(20., player_l->_resources.at(ResourceType::Food), 1e-5);
}
