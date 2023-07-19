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

	EXPECT_NEAR(10., to_double(state_l.getEntity(0)->_hp), 1e-5);

	SteppableData const * data_l = step_l.newData(state_l);
	step_l.apply(state_l);

	EXPECT_NEAR(10., to_double(state_l.getEntity(0)->_hp), 1e-5);

	step_l.revert(state_l, data_l);
	delete data_l;

	EXPECT_NEAR(10., to_double(state_l.getEntity(0)->_hp), 1e-5);
}

TEST(entityHitPointChangeTest, simple_overkill)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());

	octopus::EntityModel unitModel_l { false, 1., 1., 10. };
	state_l.addEntity(new Entity { { 3, 3. }, false, unitModel_l});

	EntityHitPointChangeStep step_l(0, -100.);

	EXPECT_NEAR(10., to_double(state_l.getEntity(0)->_hp), 1e-5);

	SteppableData const * data_l = step_l.newData(state_l);
	step_l.apply(state_l);

	EXPECT_NEAR(0., to_double(state_l.getEntity(0)->_hp), 1e-5);

	step_l.revert(state_l, data_l);
	delete data_l;

	EXPECT_NEAR(10., to_double(state_l.getEntity(0)->_hp), 1e-5);
}

TEST(entityHitPointChangeTest, simple_buff_heal)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());

	octopus::EntityModel unitModel_l { false, 1., 1., 10. };
	state_l.addEntity(new Entity { { 3, 3. }, false, unitModel_l});


	EntityHitPointChangeStep step_l(0, 5);

	EXPECT_NEAR(10., to_double(state_l.getEntity(0)->_hp), 1e-5);

	SteppableData const * data_l = step_l.newData(state_l);
	step_l.apply(state_l);

	EXPECT_NEAR(10., to_double(state_l.getEntity(0)->_hp), 1e-5);

	step_l.revert(state_l, data_l);
	delete data_l;

	EXPECT_NEAR(10., to_double(state_l.getEntity(0)->_hp), 1e-5);
}

TEST(entityHitPointChangeTest, heal_after_death_same_step)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());

	octopus::EntityModel unitModel_l { false, 1., 1., 10. };
	state_l.addEntity(new Entity { { 3, 3. }, false, unitModel_l});

	EntityHitPointChangeStep step1_l(0, -10);
	EntityHitPointChangeStep step2_l(0, 10);

	EXPECT_NEAR(10., to_double(state_l.getEntity(0)->_hp), 1e-5);

	SteppableData const * data1_l = step1_l.newData(state_l);
	step1_l.apply(state_l);

	EXPECT_NEAR(0., to_double(state_l.getEntity(0)->_hp), 1e-5);

	SteppableData const * data2_l = step2_l.newData(state_l);
	step2_l.apply(state_l);

	EXPECT_NEAR(0., to_double(state_l.getEntity(0)->_hp), 1e-5);

	step2_l.revert(state_l, data2_l);
	delete data2_l;

	EXPECT_NEAR(0., to_double(state_l.getEntity(0)->_hp), 1e-5);

	step1_l.revert(state_l, data1_l);
	delete data1_l;

	EXPECT_NEAR(10., to_double(state_l.getEntity(0)->_hp), 1e-5);
}
