#include <gtest/gtest.h>

#include "step/entity/EntityHitPointChangeStep.hh"
#include "state/State.hh"
#include "state/player/Player.hh"

///
/// This test suite aims at checking that EntityHitPointChangeStep works properly
/// - change hit point but keep it in boundaries
/// - check that reverting keeps value in boundaries
/// - check change regarding buffs
///

using namespace octopus;

TEST(entityHitPointChangeTest, simple_overheal)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());

	octopus::EntityModel unitModel_l { false, 1., 1., 10. };
	state_l.addEntity(new Entity { { 3, 3. }, false, unitModel_l});

	EntityHitPointChangeStep step_l(0, 10.);
	EntityHitPointChangeStepData data_l;

	EXPECT_NEAR(10., state_l.getEntity(0)->_hp, 1e-5);

	step_l.apply(state_l, &data_l);

	EXPECT_NEAR(10., state_l.getEntity(0)->_hp, 1e-5);

	step_l.revert(state_l, &data_l);

	EXPECT_NEAR(10., state_l.getEntity(0)->_hp, 1e-5);
}

TEST(entityHitPointChangeTest, simple_overkill)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());

	octopus::EntityModel unitModel_l { false, 1., 1., 10. };
	state_l.addEntity(new Entity { { 3, 3. }, false, unitModel_l});

	EntityHitPointChangeStep step_l(0, -100.);
	EntityHitPointChangeStepData data_l;

	EXPECT_NEAR(10., state_l.getEntity(0)->_hp, 1e-5);

	step_l.apply(state_l, &data_l);

	EXPECT_NEAR(0., state_l.getEntity(0)->_hp, 1e-5);

	step_l.revert(state_l, &data_l);

	EXPECT_NEAR(10., state_l.getEntity(0)->_hp, 1e-5);
}

TEST(entityHitPointChangeTest, simple_buff_heal)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());

	octopus::EntityModel unitModel_l { false, 1., 1., 10. };
	state_l.addEntity(new Entity { { 3, 3. }, false, unitModel_l});


	EntityHitPointChangeStep step_l(0, 5);
	EntityHitPointChangeStepData data_l;

	EXPECT_NEAR(10., state_l.getEntity(0)->_hp, 1e-5);

	step_l.apply(state_l, &data_l);

	EXPECT_NEAR(10., state_l.getEntity(0)->_hp, 1e-5);

	step_l.revert(state_l, &data_l);

	EXPECT_NEAR(10., state_l.getEntity(0)->_hp, 1e-5);

	state_l.getEntity(0)->_buffHpMax._offset = 10;

	step_l.apply(state_l, &data_l);

	EXPECT_NEAR(15., state_l.getEntity(0)->_hp, 1e-5);

	step_l.revert(state_l, &data_l);

	EXPECT_NEAR(10., state_l.getEntity(0)->_hp, 1e-5);

}
