#include <gtest/gtest.h>

#include <step/entity/EntityStep.hh>
#include <state/State.hh>

///
/// This test suite aims at checking that EntityStep works properly
/// - Move an entity in a step
/// - Is correctly undone
///

using namespace octopus;

TEST(entityStepTest, simple)
{
	State state_l;

	state_l.addEntity(new Entity { { 3, 3. }, 1., false, 0});

	EntityStep step_l(0, {1, 2});

	EXPECT_NEAR(3., state_l.getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l.getEntity(0)->_pos.y, 1e-5);

	step_l.apply(state_l);

	EXPECT_NEAR(4., state_l.getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(5., state_l.getEntity(0)->_pos.y, 1e-5);

	step_l.revert(state_l);

	EXPECT_NEAR(3., state_l.getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l.getEntity(0)->_pos.y, 1e-5);
}

TEST(entityStepTest, simple_two)
{
	State state_l;

	state_l.addEntity(new Entity { { 3, 3. }, 1., false, 0});

	EntityStep step_l(0, {1, 2});
	EntityStep step2_l(0, {5, 7});

	EXPECT_NEAR(3., state_l.getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l.getEntity(0)->_pos.y, 1e-5);

	step_l.apply(state_l);
	step2_l.apply(state_l);

	EXPECT_NEAR(9., state_l.getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(12., state_l.getEntity(0)->_pos.y, 1e-5);

	step2_l.revert(state_l);
	step_l.revert(state_l);

	EXPECT_NEAR(3., state_l.getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l.getEntity(0)->_pos.y, 1e-5);
}