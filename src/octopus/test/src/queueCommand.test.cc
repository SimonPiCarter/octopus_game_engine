#include <gtest/gtest.h>

#include <command/entity/EntityMoveCommand.hh>
#include <command/entity/EntityAttackCommand.hh>
#include <controller/Controller.hh>
#include <step/entity/EntityMoveStep.hh>
#include <step/entity/spawn/EntitySpawnStep.hh>
#include <step/command/CommandQueueStep.hh>
#include <step/player/PlayerSpawnStep.hh>
#include <state/State.hh>
#include <logger/Logger.hh>

///
/// This test suite aims at checking that EntityAttackCommand works properly
/// - Move an entity to the target
/// - Attack the target then terminate
///

using namespace octopus;

TEST(queueCommandTest, simple)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 3. };

	Entity ent_l { { 11, 3. }, false, unitModel_l};
	ent_l._player = 1;

	EntitySpawnStep * spawn0_l = new EntitySpawnStep(0, Entity { { 3, 3. }, false, unitModel_l});
	EntitySpawnStep * spawn1_l = new EntitySpawnStep(1, ent_l);

	// entity 0 attack entity 1
	EntityAttackCommand * command_l = new EntityAttackCommand(0, 0, 1);
	CommandSpawnStep * commandSpawn_l = new CommandSpawnStep(command_l);

	// move back
	EntityMoveCommand * move_l = new EntityMoveCommand(0, 0, {3, 3}, 0, {{3,3}});
	move_l->setQueued(true);
	CommandSpawnStep * moveSpawn_l = new CommandSpawnStep(move_l);

	Controller controller_l({new PlayerSpawnStep(0, 0), new PlayerSpawnStep(1, 1), spawn0_l, spawn1_l, commandSpawn_l, moveSpawn_l}, 1.);

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
	EXPECT_NEAR(3., state_l->getEntity(1)->_hp, 1e-5);

	// update time to 2 seconds (5)
	controller_l.update(2.);
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// wind up should just be over but no damage still
	EXPECT_NEAR(3., state_l->getEntity(1)->_hp, 1e-5);


	// update time to 1 second (6)
	controller_l.update(1.);
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// damage has been done
	EXPECT_NEAR(0., state_l->getEntity(1)->_hp, 1e-5);


	// update time to 1 second (7)
	controller_l.update(1.);
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// we should start to move back
	EXPECT_NEAR(5., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);

}
