#include <gtest/gtest.h>

#include <command/entity/EntityBuffCommand.hh>
#include <controller/Controller.hh>
#include <step/entity/buff/EntityBuffStep.hh>
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

TEST(buffCommandTest, simple_speed)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	EntitySpawnStep * spawn0_l = new EntitySpawnStep(0, Entity { { 3, 3. }, false, unitModel_l});

	TyppedBuff buff_l;
	buff_l._duration = 10;
	buff_l._id = "speed";
	buff_l._coef = 1.;

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn0_l, new EntityBuffStep(0, buff_l)}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(1., to_double(state_l->getEntity(0)->getStepSpeed()), 1e-5);

	// update time to 1second (1)
	// buf has registered but not been applied yet
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(1., to_double(state_l->getEntity(0)->getStepSpeed()), 1e-5);

	// update time to 1 seconds (2)
	// buff has been applied
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(2., to_double(state_l->getEntity(0)->getStepSpeed()), 1e-5);

	// update time to 4 seconds (6)
	// buff is still active
	controller_l.update(6.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(2., to_double(state_l->getEntity(0)->getStepSpeed()), 1e-5);

	// Queue a new command to reset buff
	controller_l.commitCommand(new EntityBuffCommand(0, 0, buff_l));

	// update time to 5 seconds (11)
	// buff should not be reverted
	controller_l.update(5.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(2., to_double(state_l->getEntity(0)->getStepSpeed()), 1e-5);

	// update time to 16 seconds (16)
	// buff should not be reverted yet
	controller_l.update(5.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(2., to_double(state_l->getEntity(0)->getStepSpeed()), 1e-5);

	// update time to 17 seconds (17)
	// buff should now be reverted
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(1., to_double(state_l->getEntity(0)->getStepSpeed()), 1e-5);

	// Queue a new command to reset buff
	controller_l.commitCommand(new EntityBuffCommand(0, 0, buff_l));

	// update time to 5 seconds (22)
	// buff should not be reverted
	controller_l.update(5.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// should be buffed again
	EXPECT_NEAR(2., to_double(state_l->getEntity(0)->getStepSpeed()), 1e-5);
}
