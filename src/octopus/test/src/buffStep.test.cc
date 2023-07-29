#include <gtest/gtest.h>

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

TEST(buffStepTest, simple_speed)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	EntitySpawnStep * spawn0_l = new EntitySpawnStep(Handle(0), Entity { { 3, 3. }, false, unitModel_l});

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

	controller_l.update(1.);	// (1)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff has been applied
	EXPECT_NEAR(2., to_double(state_l->getEntity(Handle(0))->getStepSpeed()), 1e-5);

	controller_l.update(9.);	// (11)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff is still active
	EXPECT_NEAR(2., to_double(state_l->getEntity(Handle(0))->getStepSpeed()), 1e-5);

	controller_l.update(1.);	// (12)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff has been reverted
	EXPECT_NEAR(1., to_double(state_l->getEntity(Handle(0))->getStepSpeed()), 1e-5);
}

TEST(buffStepTest, simple_armor)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	EntitySpawnStep * spawn0_l = new EntitySpawnStep(Handle(0), Entity { { 3, 3. }, false, unitModel_l});

	TimedBuff buff_l;
	buff_l._duration = 10;
	buff_l._id = "armor";
	buff_l._coef = 0.;
	buff_l._offset = 1.;
	buff_l._type = TyppedBuff::Type::Armor;

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn0_l, new EntityBuffStep(Handle(0), buff_l)}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(1., to_double(state_l->getEntity(Handle(0))->getArmor()), 1e-5);

	controller_l.update(1.);	// (1)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buf has registered but not been applied yet
	EXPECT_NEAR(1., to_double(state_l->getEntity(Handle(0))->getArmor()), 1e-5);

	controller_l.update(1.);	// (2)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff has been applied
	EXPECT_NEAR(2., to_double(state_l->getEntity(Handle(0))->getArmor()), 1e-5);

	controller_l.update(9.);	// (11)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff is still active
	EXPECT_NEAR(2., to_double(state_l->getEntity(Handle(0))->getArmor()), 1e-5);

	controller_l.update(1.);	// (12)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff has been reverted
	EXPECT_NEAR(1., to_double(state_l->getEntity(Handle(0))->getArmor()), 1e-5);
}

TEST(buffStepTest, simple_infinite)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	EntitySpawnStep * spawn0_l = new EntitySpawnStep(Handle(0), Entity { { 3, 3. }, false, unitModel_l});

	TimedBuff buff_l;
	buff_l._duration = 0;
	buff_l._id = "armor";
	buff_l._coef = 0.;
	buff_l._offset = 1.;
	buff_l._type = TyppedBuff::Type::Armor;

	Controller controller_l({new PlayerSpawnStep(0, 0), spawn0_l, new EntityBuffStep(Handle(0), buff_l)}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(1., to_double(state_l->getEntity(Handle(0))->getArmor()), 1e-5);

	controller_l.update(1.);	// (1)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buf has registered but not been applied yet
	EXPECT_NEAR(1., to_double(state_l->getEntity(Handle(0))->getArmor()), 1e-5);

	controller_l.update(1.);	// (2)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff has been applied
	EXPECT_NEAR(2., to_double(state_l->getEntity(Handle(0))->getArmor()), 1e-5);

	controller_l.update(100.);	// (102)
	while(!controller_l.loop_body()) {}
	state_l = controller_l.queryState();

	// buff is still active
	EXPECT_NEAR(2., to_double(state_l->getEntity(Handle(0))->getArmor()), 1e-5);
}
