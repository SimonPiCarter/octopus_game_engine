#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include <command/entity/EntityAttackMoveCommand.hh>
#include <controller/Controller.hh>
#include <step/entity/EntityMoveStep.hh>
#include <step/entity/spawn/UnitSpawnStep.hh>
#include <step/player/PlayerSpawnStep.hh>
#include <step/command/CommandQueueStep.hh>
#include <state/State.hh>

///
/// This test suite aims at checking that EntityAttackMoveCommand works properly
/// when the unit is a buffing unit
/// - Move an entity along the waypoints
/// - Buff an entity along the way
///

using namespace octopus;

///
/// > 2 entities
///  - 3, 3 attack moving to 9, 3
///  - 11, 3
/// range is 3 and rays are 1
/// Entity must move from 3,3 to 6,3 to attack (3 steps) + 1 step to capture the target on 11,3
/// Entity then requires 1 step to apply the buff which then should take 1 more step to be applied
///
TEST(attackMoveBuffCommandTest, simple)
{
	octopus::UnitModel unit1Model_l { false, 1., 1., 10. };

	octopus::UnitModel unit2Model_l { false, 1., 1., 10. };
	unit2Model_l._buffer._active = true;
	unit2Model_l._buffer._reload = 10;
	unit2Model_l._buffer._range = 6;
	unit2Model_l._buffer._buff._duration = 2;
	unit2Model_l._buffer._buff._id = "speed";
	unit2Model_l._buffer._buff._coef = 1.;

	Unit ent_l { { 11, 3.5 }, false, unit1Model_l};
	ent_l._aggroDistance = 5.;

	Unit ent2_l { { 3, 3.5 }, false, unit2Model_l};
	ent2_l._aggroDistance = 5.;

	UnitSpawnStep * spawn0_l = new UnitSpawnStep(0, ent2_l);
	UnitSpawnStep * spawn1_l = new UnitSpawnStep(1, ent_l);

	// entity 0 attack entity 1
	EntityAttackMoveCommand * command_l = new EntityAttackMoveCommand(0, 0, {9, 3.5}, 0, {{9, 3.5}});
	CommandSpawnStep * commandSpawn_l = new CommandSpawnStep(command_l);

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn0_l, spawn1_l, commandSpawn_l}, 1.);

	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(3., to_double(state_l->getEntity(0)->_pos.x), 1e-5);
	EXPECT_NEAR(3.5, to_double(state_l->getEntity(0)->_pos.y), 1e-5);

	controller_l.update(1.);	// (1)

	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// unit is moving
	EXPECT_NEAR(4., to_double(state_l->getEntity(0)->_pos.x), 1e-5);
	EXPECT_NEAR(3.5, to_double(state_l->getEntity(0)->_pos.y), 1e-5);

	controller_l.update(2.);	// (3)

	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// unit is moving
	EXPECT_NEAR(6., to_double(state_l->getEntity(0)->_pos.x), 1e-5);
	EXPECT_NEAR(3.5, to_double(state_l->getEntity(0)->_pos.y), 1e-5);

	controller_l.update(2.);	// (5)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff has not been applied yet
	EXPECT_NEAR(1., to_double(state_l->getEntity(1)->getStepSpeed()), 1e-5);

	controller_l.update(1.);	// (6)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff has been applied
	EXPECT_NEAR(2., to_double(state_l->getEntity(1)->getStepSpeed()), 1e-5);

	controller_l.update(1.);	// (7)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff has been applied and is still up
	EXPECT_NEAR(2., to_double(state_l->getEntity(1)->getStepSpeed()), 1e-5);

	controller_l.update(1.);	// (8)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff has been applied and is not up anymore
	EXPECT_NEAR(1., to_double(state_l->getEntity(1)->getStepSpeed()), 1e-5);

	controller_l.update(7.);	// (15)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff has not been applied again
	EXPECT_NEAR(1., to_double(state_l->getEntity(1)->getStepSpeed()), 1e-5);

	controller_l.update(1.);	// (16)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff has NOW been applied again
	EXPECT_NEAR(2., to_double(state_l->getEntity(1)->getStepSpeed()), 1e-5);
}
