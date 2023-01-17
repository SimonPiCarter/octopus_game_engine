#include <gtest/gtest.h>

#include <command/EntityAttackCommand.hh>
#include <controller/Controller.hh>
#include <step/entity/EntityMoveStep.hh>
#include <step/entity/EntitySpawnStep.hh>
#include <step/command/CommandQueueStep.hh>
#include <state/State.hh>

///
/// This test suite aims at checking that EntityAttackCommand works properly
/// - Move an entity to the target
/// - Attack the target then terminate
///

using namespace octopus;

TEST(attackCommandTest, simple)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	EntitySpawnStep * spawn0_l = new EntitySpawnStep(Entity { { 3, 3. }, false, unitModel_l});
	EntitySpawnStep * spawn1_l = new EntitySpawnStep(Entity { { 11, 3. }, false, unitModel_l});

	// entity 0 attack entity 1
	EntityAttackCommand * command_l = new EntityAttackCommand(0, 0, 1);
	CommandSpawnStep * commandSpawn_l = new CommandSpawnStep(command_l);

	Controller controller_l({spawn0_l, spawn1_l, commandSpawn_l}, 1.);

	State const *a = controller_l.getBackState();
	State const *b = controller_l.getBufferState();
	State const *c = controller_l.getFrontState();

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);

	// update time to 1second (1)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(4., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);

	// update time to 2 seconds (3)
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(6., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);
	EXPECT_NEAR(10., state_l->getEntity(1)->_hp, 1e-5);

	// update time to 2 seconds (5)
	controller_l.update(2.);
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// wind up should just be over but no damage still
	EXPECT_NEAR(10., state_l->getEntity(1)->_hp, 1e-5);


	// update time to 1 second (6)
	controller_l.update(1.);
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// damage has been done
	EXPECT_NEAR(7., state_l->getEntity(1)->_hp, 1e-5);

	// Next damage should be -> reload time + windup
	// 10 + 3 (13)

	// update time to 12 seconds (18)
	controller_l.update(12.);
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// damage has been done
	EXPECT_NEAR(7., state_l->getEntity(1)->_hp, 1e-5);

	// update time to 1 second (19)
	controller_l.update(1.);
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// damage has been done twice
	EXPECT_NEAR(4., state_l->getEntity(1)->_hp, 1e-5);
}
