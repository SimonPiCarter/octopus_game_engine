#include <gtest/gtest.h>

#include <step/entity/EntityMoveStep.hh>
#include <step/entity/EntitySpawnStep.hh>
#include <state/State.hh>

///
/// This test suite aims at checking that EntitySpawnStep works properly
/// - Spawn an entity in a step
/// - Is correctly undone
///

using namespace octopus;

TEST(entityStepTest, simple)
{
	State state_l;

	EntitySpawnStep step_l(Entity { { 3, 4. }, 1., false});

	EXPECT_EQ(0u, state_l.getEntities().size());

	step_l.apply(state_l);

	ASSERT_EQ(1u, state_l.getEntities().size());
	EXPECT_EQ(true, state_l.getEntity(0)->_alive);
	EXPECT_NEAR(3., state_l.getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(4., state_l.getEntity(0)->_pos.y, 1e-5);

	step_l.revert(state_l);

	// was not deleted
	ASSERT_EQ(1u, state_l.getEntities().size());
	EXPECT_EQ(false, state_l.getEntity(0)->_alive);

}
