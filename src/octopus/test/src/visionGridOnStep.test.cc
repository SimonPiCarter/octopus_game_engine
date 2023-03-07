#include <gtest/gtest.h>

#include "logger/Logger.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/vision/VisionHandler.hh"
#include "step/Step.hh"
#include "step/entity/EntityMoveStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/entity/spawn/EntitySpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"

using namespace octopus;

/// this test aims at checking that vision
/// behaves correctly no matter step chaining
TEST(VisionGridOnStep, move_then_test)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._lineOfSight = 3;

	State state_l(0, 1, 10);
	VisionHandler const & handler_l = state_l.getVisionHandler();

	EXPECT_FALSE(handler_l.isVisible(0, 3, 3));

	Step initial_l(nullptr);

	initial_l.addSteppable(new PlayerSpawnStep(0, 0));
	initial_l.addSteppable(new EntitySpawnStep(0, Entity { { 3.5, 3.5 }, false, unitModel_l}));

	apply(initial_l, state_l);

	EXPECT_TRUE(handler_l.isVisible(0, 3, 3));

	Step second_l(&initial_l);

	second_l.addSteppable(new EntityMoveStep(0, Vector {2., 2.}));
	second_l.addSteppable(new EntityHitPointChangeStep(0, -10.));

	apply(second_l, state_l);

	for(unsigned long i = 0 ; i < 10; ++i)
	{
		for(unsigned long j = 0 ; j < 10; ++j)
		{
			EXPECT_FALSE(handler_l.isVisible(0, i, j)) << "expected no vision on "<<i << ", "<<j;
		}
	}
}

TEST(VisionGridOnStep, death_then_move)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._lineOfSight = 3;

	State state_l(0, 1, 10);
	VisionHandler const & handler_l = state_l.getVisionHandler();

	EXPECT_FALSE(handler_l.isVisible(0, 3, 3));

	Step initial_l(nullptr);

	initial_l.addSteppable(new PlayerSpawnStep(0, 0));
	initial_l.addSteppable(new EntitySpawnStep(0, Entity { { 3.5, 3.5 }, false, unitModel_l}));

	apply(initial_l, state_l);

	EXPECT_TRUE(handler_l.isVisible(0, 3, 3));

	Step second_l(&initial_l);

	second_l.addSteppable(new EntityHitPointChangeStep(0, -10.));
	second_l.addSteppable(new EntityMoveStep(0, Vector {2., 2.}));

	apply(second_l, state_l);

	for(unsigned long i = 0 ; i < 10; ++i)
	{
		for(unsigned long j = 0 ; j < 10; ++j)
		{
			EXPECT_FALSE(handler_l.isVisible(0, i, j)) << "expected no vision on "<<i << ", "<<j;
		}
	}
}
