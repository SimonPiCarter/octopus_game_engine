#include <gtest/gtest.h>

#include <command/entity/EntityBuffCommand.hh>
#include <controller/Controller.hh>
#include <step/entity/buff/EntityBuffStep.hh>
#include <step/entity/spawn/UnitSpawnStep.hh>
#include <step/command/CommandQueueStep.hh>
#include <step/player/PlayerSpawnStep.hh>
#include <state/State.hh>

///
/// This test suite aims at checking that EntityBuffCommand works properly
///

using namespace octopus;

TEST(buffCommandTest, simple_speed)
{
	octopus::UnitModel unitModel_l { false, 1., 1., 10. };

	UnitSpawnStep * spawn0_l = new UnitSpawnStep(Handle(0), Unit { { 3, 3. }, false, unitModel_l});

	TimedBuff buff_l;
	buff_l._duration = 10;
	buff_l._id = "speed";
	buff_l._coef = 1.;

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn0_l, new EntityBuffStep(Handle(0), buff_l)}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(1., to_double(state_l->getEntity(Handle(0))->getStepSpeed()), 1e-5);

	controller_l.update(1.);	// (1)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buf has registered but not been applied yet
	EXPECT_NEAR(1., to_double(state_l->getEntity(Handle(0))->getStepSpeed()), 1e-5);

	controller_l.update(1.);	// (2)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff has been applied
	EXPECT_NEAR(2., to_double(state_l->getEntity(Handle(0))->getStepSpeed()), 1e-5);

	controller_l.update(6.);	// (6)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff is still active
	EXPECT_NEAR(2., to_double(state_l->getEntity(Handle(0))->getStepSpeed()), 1e-5);

	// Queue a new command to reset buff
	controller_l.commitCommand(new EntityBuffCommand(Handle(0), Handle(0), buff_l));

	controller_l.update(5.);	// (11)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff should not be reverted
	EXPECT_NEAR(2., to_double(state_l->getEntity(Handle(0))->getStepSpeed()), 1e-5);

	controller_l.update(6.);	// (17)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff should not be reverted yet
	EXPECT_NEAR(2., to_double(state_l->getEntity(Handle(0))->getStepSpeed()), 1e-5);

	controller_l.update(1.);	// (18)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff should now be reverted
	EXPECT_NEAR(1., to_double(state_l->getEntity(Handle(0))->getStepSpeed()), 1e-5);

	// Queue a new command to reset buff
	controller_l.commitCommand(new EntityBuffCommand(Handle(0), Handle(0), buff_l));

	controller_l.update(5.);	// (23)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff should not be reverted
	// should be buffed again
	EXPECT_NEAR(2., to_double(state_l->getEntity(Handle(0))->getStepSpeed()), 1e-5);
}
