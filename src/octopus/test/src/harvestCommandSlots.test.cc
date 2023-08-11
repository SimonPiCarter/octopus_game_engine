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
/// This test suite aims at checking that Harvest command works properly especially with harvest slots
///

using namespace octopus;

///
/// > 3 entities
///  - unit : 5 3 harvesting
///  - building : 1, 3 deposit for Food
///  - resource : 10, 3 resource of food (20 food)
///
TEST(harvestCommandSlotsTest, simple)
{
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._maxQuantity["bloc"] = 10;
	Unit unit_l({ 5, 3 }, false, unitModel_l);

	ResourceModel resModel_l(1., "bloc", 20);
	resModel_l._harvestPoints.push_back({-3,0});
	Resource res_l({10,3}, true, resModel_l);

	BuildingModel depositModel_l { true, 1., 10. };
	depositModel_l._isBuilding = true;
	depositModel_l._deposit["bloc"] = true;

	Building deposit_l({1,3}, true, depositModel_l);

	UnitSpawnStep * spawn0_l = new UnitSpawnStep(Handle(0), unit_l);
	ResourceSpawnStep * spawn1_l = new ResourceSpawnStep(Handle(1), res_l);
	BuildingSpawnStep * spawn2_l = new BuildingSpawnStep(Handle(2), deposit_l, true);

	// unit harvest
	UnitHarvestCommand * command_l = new UnitHarvestCommand(Handle(0), Handle(0), Handle(1), res_l._pos, 0, {res_l._pos});
	CommandSpawnStep * commandSpawn_l = new CommandSpawnStep(command_l);

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn0_l, spawn1_l, spawn2_l, commandSpawn_l}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(5, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

	// update time to 1second (1)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(6, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

	// update time to 1 seconds (2)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(7, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

	// update time to 1 seconds (3)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	Unit const * stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));

	EXPECT_NEAR(7, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(1., to_double(stateUnit_l->_quantityOfResource), 1e-5);

	// update time to 9 seconds (12)
	controller_l.update(9.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));

	EXPECT_NEAR(7, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(10., to_double(stateUnit_l->_quantityOfResource), 1e-5);

	// update time to 2 seconds (14) : scanning for deposit + moving
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));

	EXPECT_NEAR(6, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(10., to_double(stateUnit_l->_quantityOfResource), 1e-5);

	// update time to 3 seconds (17)
	controller_l.update(3.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));

	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(10., to_double(stateUnit_l->_quantityOfResource), 1e-5);

	// update time to 1 seconds (18)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));
	Player const * player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(0., to_double(stateUnit_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(10., to_double(getResource(*player_l, "bloc")), 1e-5);

	// update time to 19 seconds (27) : full trip just before trop
	// 4 steps : move to harvest
	// 10 steps : harvest 10
	// 1 step : scan for depoit
	// 4 steps : move to deposit
	controller_l.update(19.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));
	player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(10., to_double(stateUnit_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(10., to_double(getResource(*player_l, "bloc")), 1e-5);

	// update time to 1 seconds (28) : drop
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));
	player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(0., to_double(stateUnit_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(20., to_double(getResource(*player_l, "bloc")), 1e-5);

	// no more resurce to harvest stop here

	// update time to 10 seconds (38) : drop
	controller_l.update(10.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));
	player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(0., to_double(stateUnit_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(20., to_double(getResource(*player_l, "bloc")), 1e-5);
}

///
/// > 4 entities
///  - unit : 5 3 harvesting
///  - unit : 4 3 harvesting
///  - building : 1, 3 deposit for Food
///  - resource : 11, 3 resource of food (40 food)
///
TEST(harvestCommandSlotsTest, simple_two_slots)
{
	// to avoid flow field to interfer
	PathManager::ForceLoSCheck = true;
	UnitModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._isUnit = true;
	unitModel_l._maxQuantity["bloc"] = 10;
	Unit unit1_l({ 5, 3 }, false, unitModel_l);
	Unit unit2_l({ 4, 3 }, false, unitModel_l);

	ResourceModel resModel_l(1., "bloc", 40);
	resModel_l._harvestPoints.push_back({-4,0});
	resModel_l._harvestPoints.push_back({-3,0});
	Resource res_l({11,3}, true, resModel_l);

	BuildingModel depositModel_l { true, 1., 10. };
	depositModel_l._isBuilding = true;
	depositModel_l._deposit["bloc"] = true;

	Building deposit_l({1,3}, true, depositModel_l);

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new UnitSpawnStep(Handle(0), unit1_l),
		new ResourceSpawnStep(Handle(1), res_l),
		new BuildingSpawnStep(Handle(2), deposit_l, true),
		new UnitSpawnStep(Handle(3), unit2_l),
		new CommandSpawnStep(new UnitHarvestCommand(Handle(0), Handle(0), Handle(1), res_l._pos, 0, {res_l._pos})),
		new CommandSpawnStep(new UnitHarvestCommand(Handle(3), Handle(3), Handle(1), res_l._pos, 0, {res_l._pos})),
		}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(5, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_NEAR(4, to_double(state_l->getEntity(Handle(3))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(3))->_pos.y), 1e-5);

	// update time to 1second (1)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(6, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_NEAR(5, to_double(state_l->getEntity(Handle(3))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(3))->_pos.y), 1e-5);

	// update time to 1 seconds (2)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(7, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_NEAR(6, to_double(state_l->getEntity(Handle(3))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(3))->_pos.y), 1e-5);

	// update time to 1 seconds (3)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	Unit const * stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));
	Unit const * stateUnit2_l = static_cast<Unit const *>(state_l->getEntity(Handle(3)));

	EXPECT_NEAR(7, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(1., to_double(stateUnit_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(7, to_double(state_l->getEntity(Handle(3))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(3))->_pos.y), 1e-5);

	// update time to 2 seconds (5)
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));
	stateUnit2_l = static_cast<Unit const *>(state_l->getEntity(Handle(3)));

	EXPECT_NEAR(7, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(3., to_double(stateUnit_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(8, to_double(state_l->getEntity(Handle(3))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(3))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit2_l->_typeOfResource);
	EXPECT_NEAR(0., to_double(stateUnit2_l->_quantityOfResource), 1e-5);

	// update time to 7 seconds (12)
	controller_l.update(7.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));
	stateUnit2_l = static_cast<Unit const *>(state_l->getEntity(Handle(3)));

	EXPECT_NEAR(7, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(10., to_double(stateUnit_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(8, to_double(state_l->getEntity(Handle(3))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(3))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit2_l->_typeOfResource);
	EXPECT_NEAR(7., to_double(stateUnit2_l->_quantityOfResource), 1e-5);

	// update time to 2 seconds (14) : scanning for deposit + moving
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));
	stateUnit2_l = static_cast<Unit const *>(state_l->getEntity(Handle(3)));

	EXPECT_NEAR(6, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(10., to_double(stateUnit_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(8, to_double(state_l->getEntity(Handle(3))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(3))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit2_l->_typeOfResource);
	EXPECT_NEAR(9., to_double(stateUnit2_l->_quantityOfResource), 1e-5);

	// update time to 3 seconds (17)
	controller_l.update(3.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));
	stateUnit2_l = static_cast<Unit const *>(state_l->getEntity(Handle(3)));

	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(10., to_double(stateUnit_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(7, to_double(state_l->getEntity(Handle(3))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(3))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit2_l->_typeOfResource);
	EXPECT_NEAR(10., to_double(stateUnit2_l->_quantityOfResource), 1e-5);

	// update time to 1 seconds (18)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));
	stateUnit2_l = static_cast<Unit const *>(state_l->getEntity(Handle(3)));
	Player const * player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(0., to_double(stateUnit_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(6, to_double(state_l->getEntity(Handle(3))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(3))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit2_l->_typeOfResource);
	EXPECT_NEAR(10., to_double(stateUnit2_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(10., to_double(getResource(*player_l, "bloc")), 1e-5);

	// update time to 19 seconds (37) : full trip just before trop
	// 4 steps : move to harvest
	// 10 steps : harvest 10
	// 1 step : scan for depoit
	// 4 steps : move to deposit
	controller_l.update(19.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));
	stateUnit2_l = static_cast<Unit const *>(state_l->getEntity(Handle(3)));
	player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(10., to_double(stateUnit_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(8, to_double(state_l->getEntity(Handle(3))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(3))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit2_l->_typeOfResource);
	EXPECT_NEAR(9., to_double(stateUnit2_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(20., to_double(getResource(*player_l, "bloc")), 1e-5);

	// update time to 1 seconds (38) : drop
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));
	stateUnit2_l = static_cast<Unit const *>(state_l->getEntity(Handle(3)));
	player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(0., to_double(stateUnit_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(8, to_double(state_l->getEntity(Handle(3))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(3))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit2_l->_typeOfResource);
	EXPECT_NEAR(10., to_double(stateUnit2_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(30., to_double(getResource(*player_l, "bloc")), 1e-5);

	// no more resurce to harvest stop here

	// update time to 10 seconds (48) : drop
	controller_l.update(10.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	stateUnit_l = static_cast<Unit const *>(state_l->getEntity(Handle(0)));
	stateUnit2_l = static_cast<Unit const *>(state_l->getEntity(Handle(3)));
	player_l = state_l->getPlayer(0);

	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit_l->_typeOfResource);
	EXPECT_NEAR(0., to_double(stateUnit_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(3))->_pos.x), 1e-5);
	EXPECT_NEAR(3, to_double(state_l->getEntity(Handle(3))->_pos.y), 1e-5);
	EXPECT_EQ("bloc", stateUnit2_l->_typeOfResource);
	EXPECT_NEAR(0., to_double(stateUnit2_l->_quantityOfResource), 1e-5);
	EXPECT_NEAR(40., to_double(getResource(*player_l, "bloc")), 1e-5);

	// restore param
	PathManager::ForceLoSCheck = false;
}
