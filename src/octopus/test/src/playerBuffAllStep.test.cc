#include <gtest/gtest.h>

#include "step/TickingStep.hh"
#include "step/player/PlayerBuffAllStep.hh"
#include "step/entity/spawn/EntitySpawnStep.hh"
#include "state/State.hh"
#include "state/player/Player.hh"
#include "state/entity/Entity.hh"

///
/// This test suite aims at checking that PlayerBuffAllStep works properly
/// - check that only given player is affected
/// - check that only model is affected
/// - check that revert is correctly applied
/// - check that it applies to spawned unit after buff has been applied
///

using namespace octopus;

TEST(PlayerBuffAllStepStepTest, simple)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());

	EntityModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._id = "unit";
	state_l.addEntity(new Entity { { 3, 3. }, false, unitModel_l});
	state_l.getEntity(0)->_player = 0;

	TyppedBuff buff_l;
	buff_l._type = TyppedBuff::Type::HpMax;
	buff_l._offset = 10;
	PlayerBuffAllStep step_l(0, buff_l, "unit");

	// required to apply buff or revert
	TickingStep tickStep_l;

	EXPECT_NEAR(10., state_l.getEntity(0)->getHpMax(), 1e-5);

	SteppableData *data_l = step_l.newData(state_l);
	step_l.apply(state_l);
	tickStep_l.apply(state_l);

	EXPECT_NEAR(20., state_l.getEntity(0)->getHpMax(), 1e-5);

	tickStep_l.revert(state_l, nullptr);
	step_l.revert(state_l, data_l);
	delete data_l;
	tickStep_l.apply(state_l);

	EXPECT_NEAR(10., state_l.getEntity(0)->getHpMax(), 1e-5);
}

TEST(PlayerBuffAllStepStepTest, simple_wrong_player)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());

	EntityModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._id = "unit";
	state_l.addEntity(new Entity { { 3, 3. }, false, unitModel_l});
	state_l.getEntity(0)->_player = 1;

	TyppedBuff buff_l;
	buff_l._type = TyppedBuff::Type::HpMax;
	buff_l._offset = 10;
	PlayerBuffAllStep step_l(0, buff_l, "unit");

	// required to apply buff or revert
	TickingStep tickStep_l;

	EXPECT_NEAR(10., state_l.getEntity(0)->getHpMax(), 1e-5);

	SteppableData *data_l = step_l.newData(state_l);
	step_l.apply(state_l);
	tickStep_l.apply(state_l);

	EXPECT_NEAR(10., state_l.getEntity(0)->getHpMax(), 1e-5);

	tickStep_l.revert(state_l, nullptr);
	step_l.revert(state_l, data_l);
	delete data_l;
	tickStep_l.apply(state_l);

	EXPECT_NEAR(10., state_l.getEntity(0)->getHpMax(), 1e-5);
}

TEST(PlayerBuffAllStepStepTest, simple_wrong_model)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());

	EntityModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._id = "unit";
	state_l.addEntity(new Entity { { 3, 3. }, false, unitModel_l});
	state_l.getEntity(0)->_player = 0;

	TyppedBuff buff_l;
	buff_l._type = TyppedBuff::Type::HpMax;
	buff_l._offset = 10;
	PlayerBuffAllStep step_l(0, buff_l, "unit2");

	// required to apply buff or revert
	TickingStep tickStep_l;

	EXPECT_NEAR(10., state_l.getEntity(0)->getHpMax(), 1e-5);

	SteppableData *data_l = step_l.newData(state_l);
	step_l.apply(state_l);
	tickStep_l.apply(state_l);

	EXPECT_NEAR(10., state_l.getEntity(0)->getHpMax(), 1e-5);

	tickStep_l.revert(state_l, nullptr);
	step_l.revert(state_l, data_l);
	delete data_l;
	tickStep_l.apply(state_l);

	EXPECT_NEAR(10., state_l.getEntity(0)->getHpMax(), 1e-5);
}

TEST(PlayerBuffAllStepStepTest, spawn)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());

	EntityModel unitModel_l { false, 1., 1., 10. };
	unitModel_l._id = "unit";
	state_l.addEntity(new Entity { { 3, 3. }, false, unitModel_l});
	state_l.getEntity(0)->_player = 0;

	TyppedBuff buff_l;
	buff_l._type = TyppedBuff::Type::HpMax;
	buff_l._offset = 10;
	PlayerBuffAllStep step_l(0, buff_l, "unit");
	PlayerBuffAllStepData data_l;

	// required to apply buff or revert
	TickingStep tickStep_l;

	EXPECT_NEAR(10., state_l.getEntity(0)->getHpMax(), 1e-5);

	step_l.apply(state_l);
	tickStep_l.apply(state_l);

	EXPECT_NEAR(20., state_l.getEntity(0)->getHpMax(), 1e-5);

	EntitySpawnStep spawnStep_l(1, *state_l.getEntity(0));

	spawnStep_l.apply(state_l);

	EXPECT_NEAR(20., state_l.getEntity(1)->getHpMax(), 1e-5);

}
