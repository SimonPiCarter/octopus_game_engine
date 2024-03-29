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

	EntitySpawnStep * spawn0_l = new EntitySpawnStep(Handle(0), Entity { { 3, 3. }, false, unitModel_l});
	EntitySpawnStep * spawn1_l = new EntitySpawnStep(Handle(1), ent_l);

	// entity 0 attack entity 1
	EntityAttackCommand * command_l = new EntityAttackCommand(Handle(0), Handle(0), Handle(1), true);
	CommandSpawnStep * commandSpawn_l = new CommandSpawnStep(command_l);

	// move back
	EntityMoveCommand * move_l = new EntityMoveCommand(Handle(0), Handle(0), {3, 3}, 0, {{3,3}});
	move_l->setQueued(true);
	CommandSpawnStep * moveSpawn_l = new CommandSpawnStep(move_l);

	Controller controller_l({new PlayerSpawnStep(0, 0), new PlayerSpawnStep(1, 1), spawn0_l, spawn1_l, commandSpawn_l, moveSpawn_l}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

	controller_l.update(2.); // (2)

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(4., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

	controller_l.update(2.); // (4)

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(6., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);

	controller_l.update(2.); // (6)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// wind up should just be over but no damage still
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);


	controller_l.update(1.); // (7)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// damage has been done
	EXPECT_NEAR(0., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);


	controller_l.update(1.); // (8)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// we should start to move back
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

}
