#include <gtest/gtest.h>

#include <command/entity/EntityAttackCommand.hh>
#include <controller/Controller.hh>
#include <step/entity/EntityMoveStep.hh>
#include <step/entity/spawn/EntitySpawnStep.hh>
#include <step/command/CommandQueueStep.hh>
#include <step/player/PlayerSpawnStep.hh>
#include <state/State.hh>

///
/// This test suite aims at checking that EntityAttackCommand works properly
/// - Move an entity to the target
/// - Attack the target then terminate
///

using namespace octopus;

TEST(attackCommandHealTest, simple)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._damage = 0;
	unitModel_l._heal = 1;

	EntitySpawnStep * spawn0_l = new EntitySpawnStep(Handle(0), Entity { { 3, 3. }, false, unitModel_l});
	Entity ent2_l { { 11, 3. }, false, unitModel_l};
	ent2_l._hp = 5;
	EntitySpawnStep * spawn1_l = new EntitySpawnStep(Handle(1), ent2_l);

	// entity 0 heal entity 1
	EntityAttackCommand * command_l = new EntityAttackCommand(Handle(0), Handle(0), Handle(1), true);
	CommandSpawnStep * commandSpawn_l = new CommandSpawnStep(command_l);

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn0_l, spawn1_l, commandSpawn_l}, 1);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

	// update time to 1second (1)
	controller_l.update(1);
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	EXPECT_NEAR(4., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

	// update time to 2 seconds (3)
	controller_l.update(2);
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	EXPECT_NEAR(6., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);

	// update time to 2 seconds (5)
	controller_l.update(2);
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// wind up should just be over but no damage still
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);


	// update time to 1 second (6)
	controller_l.update(1);
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// heal has been done
	EXPECT_NEAR(6., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);

	// Next heal should be -> reload time + windup
	// 10 + 3 (13)

	// update time to 12 seconds (18)
	controller_l.update(12);
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// heal has been done
	EXPECT_NEAR(6., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);

	// update time to 1 second (19)
	controller_l.update(1);
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// heal has been done twice
	EXPECT_NEAR(7., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);
}
