#include <gtest/gtest.h>

#include "step/entity/EntityMoveStep.hh"
#include "step/entity/spawn/EntitySpawnStep.hh"
#include "state/State.hh"
#include "state/player/Player.hh"

///
/// This test suite aims at checking that EntitySpawnStep works properly
/// - Spawn an entity in a step
/// - Is correctly undone
///

using namespace octopus;

TEST(entityStepTest, simple)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());

	octopus::EntityModel unitModel_l { false, 1., 1., 10. };
	EntitySpawnStep step_l(0, Entity { { 3, 4. }, false, unitModel_l});

	EXPECT_EQ(0u, state_l.getEntities().size());

	step_l.apply(state_l, nullptr);

	ASSERT_EQ(1u, state_l.getEntities().size());
	EXPECT_EQ(true, state_l.getEntity(0)->_alive);
	EXPECT_NEAR(3., to_double(state_l.getEntity(0)->_pos.x), 1e-5);
	EXPECT_NEAR(4., to_double(state_l.getEntity(0)->_pos.y), 1e-5);

	step_l.revert(state_l, nullptr);

	// was not deleted
	ASSERT_EQ(1u, state_l.getEntities().size());
	EXPECT_EQ(false, state_l.getEntity(0)->_alive);

}
