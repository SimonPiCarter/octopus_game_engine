#include <gtest/gtest.h>

#include <command/EntityMoveCommand.hh>
#include <controller/Controller.hh>
#include <step/entity/EntityMoveStep.hh>
#include <step/entity/EntitySpawnStep.hh>
#include <step/command/CommandQueueStep.hh>
#include <state/State.hh>

///
/// This test suite aims at checking that EntityMoveStep works properly
/// - Move an entity in a step
/// - Is correctly undone
///

using namespace octopus;

TEST(controlerTest, simple)
{
	EntitySpawnStep * spawn_l = new EntitySpawnStep(Entity { { 3, 3. }, 1., false, 1});
	EntityMoveCommand * command_l = new EntityMoveCommand(0, 0, { {4, 3}, {4, 4}});
	CommandSpawnStep * commandSpawn_l = new CommandSpawnStep(0, command_l, false);

	Controller controller_l({spawn_l, commandSpawn_l}, 1.);

	State const *a = controller_l.getBackState();
	State const *b = controller_l.getBufferState();
	State const *c = controller_l.getFrontState();

	// here all state are on step 1 and nothing is to be done because we are on time 0

	// controller internals :
	// back : 1 (a)
	// buffer : 1 (b)
	// front : 1 (c)
	EXPECT_EQ(a, controller_l.getBackState());
	EXPECT_EQ(b, controller_l.getBufferState());
	EXPECT_EQ(c, controller_l.getFrontState());

	EXPECT_TRUE(controller_l.loop_body());

	// controller internals :
	// back : 1 (a)
	// buffer : 1 (b)
	// front : 1 (c)
	EXPECT_EQ(a, controller_l.getBackState());
	EXPECT_EQ(b, controller_l.getBufferState());
	EXPECT_EQ(c, controller_l.getFrontState());

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);

	// update time to 1second
	controller_l.update(1.);

	// Here back state should have been updated and swapped with buffered state
	EXPECT_FALSE(controller_l.loop_body());

	// controller internals :
	// back : 1 (b)
	// buffer : 2 (a)
	// front : 1 (c)
	EXPECT_EQ(b, controller_l.getBackState());
	EXPECT_EQ(a, controller_l.getBufferState());
	EXPECT_EQ(c, controller_l.getFrontState());

	// query state (here we should be on step 2)
	state_l = controller_l.queryState();

	// controller internals :
	// back : 1 (b)
	// buffer : 1 (c)
	// front : 2 (a)
	EXPECT_EQ(b, controller_l.getBackState());
	EXPECT_EQ(c, controller_l.getBufferState());
	EXPECT_EQ(a, controller_l.getFrontState());

	EXPECT_NEAR(4., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);

	// update time to 1second
	controller_l.update(1.);
	EXPECT_FALSE(controller_l.loop_body());

	// controller internals :
	// back : 1 (c)
	// buffer : 2 (b)
	// front : 2 (a)
	EXPECT_EQ(c, controller_l.getBackState());
	EXPECT_EQ(b, controller_l.getBufferState());
	EXPECT_EQ(a, controller_l.getFrontState());

	// query state (should not swap)
	state_l = controller_l.queryState();

	// controller internals :
	// back : 1 (c)
	// buffer : 2 (b)
	// front : 2 (a)
	EXPECT_EQ(c, controller_l.getBackState());
	EXPECT_EQ(b, controller_l.getBufferState());
	EXPECT_EQ(a, controller_l.getFrontState());

	EXPECT_NEAR(4., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);

	EXPECT_FALSE(controller_l.loop_body());

	// controller internals :
	// back : 2 (c)
	// buffer : 2 (b)
	// front : 2 (a)
	EXPECT_EQ(c, controller_l.getBackState());
	EXPECT_EQ(b, controller_l.getBufferState());
	EXPECT_EQ(a, controller_l.getFrontState());

	// query state (should not swap)
	state_l = controller_l.queryState();

	// controller internals :
	// back : 2 (c)
	// buffer : 2 (b)
	// front : 2 (a)
	EXPECT_EQ(c, controller_l.getBackState());
	EXPECT_EQ(b, controller_l.getBufferState());
	EXPECT_EQ(a, controller_l.getFrontState());

	EXPECT_NEAR(4., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);

	EXPECT_FALSE(controller_l.loop_body());

	// controller internals :
	// back : 2 (b)
	// buffer : 3 (c)
	// front : 2 (a)
	EXPECT_EQ(b, controller_l.getBackState());
	EXPECT_EQ(c, controller_l.getBufferState());
	EXPECT_EQ(a, controller_l.getFrontState());

	// query state (should swap)
	state_l = controller_l.queryState();

	// controller internals :
	// back : 2 (b)
	// buffer : 2 (a)
	// front : 3 (c)
	EXPECT_EQ(b, controller_l.getBackState());
	EXPECT_EQ(a, controller_l.getBufferState());
	EXPECT_EQ(c, controller_l.getFrontState());

	EXPECT_NEAR(4., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(4., state_l->getEntity(0)->_pos.y, 1e-5);

}
