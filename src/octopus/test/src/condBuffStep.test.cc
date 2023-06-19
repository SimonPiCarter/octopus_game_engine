#include <gtest/gtest.h>

#include <step/entity/EntityHitPointChangeStep.hh>
#include <step/entity/buff/EntityBuffStep.hh>
#include <step/entity/spawn/EntitySpawnStep.hh>
#include <step/command/CommandQueueStep.hh>
#include <step/player/PlayerSpawnStep.hh>
#include <state/State.hh>

///
/// This test suite aims at checking that conditional buffs work properly
///

using namespace octopus;

// test that conditional buff correctly apply when minimum hp is required
TEST(condBuffStepTest, simple_speed_min_hp)
{
	EntityModel model_l { false, 1., 1., 10. };

	State state_l;
	state_l.addEntity(new Entity {{2,0}, false, model_l});

	ConditionalBuff buff_l;
	buff_l._id = "speed";
	buff_l._coef = 1.;
	buff_l._conditions.push_back(ConditionMinLife{1.});

	EntityConditionalBuffStep step_l(0, buff_l);
	step_l.apply(state_l);

	EXPECT_NEAR(2., to_double(state_l.getEntity(0)->getStepSpeed()), 1e-5);

	EntityHitPointChangeStep stepHp_l(0, -1, 10, 10);
	stepHp_l.apply(state_l);

	EXPECT_NEAR(1., to_double(state_l.getEntity(0)->getStepSpeed()), 1e-5);

	stepHp_l.revert(state_l, nullptr);
	EXPECT_NEAR(2., to_double(state_l.getEntity(0)->getStepSpeed()), 1e-5);
}

// test that conditional buff correctly apply when maximum hp is required
TEST(condBuffStepTest, simple_speed_max_hp)
{
	EntityModel model_l { false, 1., 1., 10. };

	State state_l;
	state_l.addEntity(new Entity {{2,0}, false, model_l});

	ConditionalBuff buff_l;
	buff_l._id = "speed";
	buff_l._coef = 1.;
	buff_l._conditions.push_back(ConditionMaxLife{0.9});

	EntityConditionalBuffStep step_l(0, buff_l);
	step_l.apply(state_l);

	EXPECT_NEAR(1., to_double(state_l.getEntity(0)->getStepSpeed()), 1e-5);

	EntityHitPointChangeStep stepHp_l(0, -1, 10, 10);
	stepHp_l.apply(state_l);

	EXPECT_NEAR(2., to_double(state_l.getEntity(0)->getStepSpeed()), 1e-5);

	stepHp_l.revert(state_l, nullptr);
	EXPECT_NEAR(1., to_double(state_l.getEntity(0)->getStepSpeed()), 1e-5);
}
