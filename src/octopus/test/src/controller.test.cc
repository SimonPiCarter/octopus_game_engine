#include <gtest/gtest.h>

#include <command/entity/EntityMoveCommand.hh>
#include <controller/Controller.hh>
#include <step/command/CommandQueueStep.hh>
#include <step/entity/EntityMoveStep.hh>
#include <step/entity/spawn/EntitySpawnStep.hh>
#include <step/player/PlayerSpawnStep.hh>
#include <state/State.hh>

///
/// This test suite aims at checking that EntityMoveStep works properly
/// - Move an entity in a step
/// - Is correctly undone
///

using namespace octopus;

TEST(controllerTest, simple)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	EntitySpawnStep * spawn_l = new EntitySpawnStep(Handle(0), Entity { { 3, 3. }, false, unitModel_l});
	EntityMoveCommand * command_l = new EntityMoveCommand(Handle(0), Handle(0), {4, 3}, 0, { {4, 3}});
	EntityMoveCommand * command2_l = new EntityMoveCommand(Handle(0), Handle(0), {4, 4}, 0, { {4, 4}});
	command2_l->setQueued(true);
	CommandSpawnStep * commandSpawn_l = new CommandSpawnStep(command_l);
	CommandSpawnStep * commandSpawn2_l = new CommandSpawnStep(command2_l);

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn_l, commandSpawn_l, commandSpawn2_l}, 1.);

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

	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

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

	EXPECT_NEAR(4., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

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

	EXPECT_NEAR(4., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

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

	EXPECT_NEAR(4., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

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

	EXPECT_NEAR(4., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(4., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

}
