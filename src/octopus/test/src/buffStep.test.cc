#include <gtest/gtest.h>

#include <controller/Controller.hh>
#include <step/entity/buff/EntityBuffStep.hh>
#include <step/entity/spawn/EntitySpawnStep.hh>
#include <step/command/CommandQueueStep.hh>
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

	EntitySpawnStep * spawn0_l = new EntitySpawnStep(Entity { { 3, 3. }, false, unitModel_l});

	TyppedBuff buff_l;
	buff_l._duration = 10;
	buff_l._id = "speed";
	buff_l._coef = 1.;

	Controller controller_l({spawn0_l, new EntityBuffStep(0, buff_l, 0, true)}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(1., state_l->getEntity(0)->getStepSpeed(), 1e-5);

	// update time to 1second (1)
	// buf has registered but not been applied yet
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(1., state_l->getEntity(0)->getStepSpeed(), 1e-5);

	// update time to 1 seconds (2)
	// buff has been applied
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(2., state_l->getEntity(0)->getStepSpeed(), 1e-5);

	// update time to 8 seconds (10)
	// buff is still active
	controller_l.update(8.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(2., state_l->getEntity(0)->getStepSpeed(), 1e-5);

	// update time to 1 seconds (11)
	// buhf has been reverted
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(1., state_l->getEntity(0)->getStepSpeed(), 1e-5);
}

TEST(buffStepTest, simple_armor)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	EntitySpawnStep * spawn0_l = new EntitySpawnStep(Entity { { 3, 3. }, false, unitModel_l});

	TyppedBuff buff_l;
	buff_l._duration = 10;
	buff_l._id = "armor";
	buff_l._coef = 0.;
	buff_l._offset = 1.;
	buff_l._type = TyppedBuff::Type::Armor;

	Controller controller_l({spawn0_l, new EntityBuffStep(0, buff_l, 0, true)}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(1., state_l->getEntity(0)->getArmor(), 1e-5);

	// update time to 1second (1)
	// buf has registered but not been applied yet
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(1., state_l->getEntity(0)->getArmor(), 1e-5);

	// update time to 1 seconds (2)
	// buff has been applied
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(2., state_l->getEntity(0)->getArmor(), 1e-5);

	// update time to 8 seconds (10)
	// buff is still active
	controller_l.update(8.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(2., state_l->getEntity(0)->getArmor(), 1e-5);

	// update time to 1 seconds (11)
	// buhf has been reverted
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(1., state_l->getEntity(0)->getArmor(), 1e-5);
}
