#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include <command/entity/EntityAttackCommand.hh>
#include <controller/Controller.hh>
#include <step/entity/spawn/UnitSpawnStep.hh>
#include <step/command/CommandQueueStep.hh>
#include <step/player/PlayerSpawnStep.hh>
#include <state/State.hh>
#include <state/model/entity/UnitModel.hh>
#include <state/entity/Buff.hh>

///
/// This test suite aims at checking that EntityBuffCommand is correctly output
///
/// Test that multiple entities are buffed with the correct interval
/// and that filter on non same team and too far works properly (ent4 and ent5)
///

using namespace octopus;

TEST(autoBuffCommandTest, simple)
{
	UnitModel unitModel1_l { false, 1., 1., 10. };
	unitModel1_l._damage = 0.;
	unitModel1_l._range = 1.;

	// should buff (speed+1) x2
	unitModel1_l._buffer._buff._duration = 10;
	unitModel1_l._buffer._buff._id = "speed";
	unitModel1_l._buffer._buff._offset = 1;
	unitModel1_l._buffer._buff._coef = 1;
	unitModel1_l._buffer._range = 10.;
	unitModel1_l._buffer._reload = 3;
	unitModel1_l._buffer._active = true;

	UnitModel unitModel2_l { false, 1., 1., 10. };
	unitModel2_l._damage = 0.;
	unitModel2_l._range = 1.;
	unitModel2_l._stepSpeed = 1.;

	Unit ent1_l { { 3, 3. }, false, unitModel1_l};
	ent1_l._player = 0;

	// The two unit to be buffed
	Unit ent2_l { { 8, 3. }, false, unitModel2_l};
	ent2_l._player = 0;
	Unit ent3_l { { 10, 3. }, false, unitModel2_l};
	ent3_l._player = 0;

	// this unit should not be buffed since it is a different player/teeam
	Unit ent4_l { { 5, 3. }, false, unitModel2_l};
	ent4_l._player = 1;

	// This unit should not be buffed because too far away
	Unit ent5_l { { 15, 3. }, false, unitModel2_l};
	ent5_l._player = 0;

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new UnitSpawnStep(ent1_l),
		new UnitSpawnStep(ent2_l),
		new UnitSpawnStep(ent3_l),
		new UnitSpawnStep(ent4_l),
		new UnitSpawnStep(ent5_l)
	}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);

	// update time to 2 second (2)
	// 1 step buff the entity (auto buff)
	// 1 step to register the buff in the entity
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// no move
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.x, 1e-5);
	EXPECT_NEAR(3., state_l->getEntity(0)->_pos.y, 1e-5);

	// not buffed
	EXPECT_NEAR(1., state_l->getEntity(0)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(1)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(2)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(3)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(4)->getStepSpeed(), 1e-5);

	// update time to 1 second (3)
	// one step to apply it (during ticking step)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// ent2 should be buffed
	EXPECT_NEAR(4., state_l->getEntity(1)->getStepSpeed(), 1e-5);
	// not buffed
	EXPECT_NEAR(1., state_l->getEntity(0)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(2)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(3)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(4)->getStepSpeed(), 1e-5);

	// update time to 2 second (5)
	// 3 steps to relaod buff
	// 1 step buff the entity (auto buff)
	// 1 step to register the buff in the entity
	controller_l.update(2.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// ent2 should be buffed
	EXPECT_NEAR(4., state_l->getEntity(1)->getStepSpeed(), 1e-5);
	// not buffed
	EXPECT_NEAR(1., state_l->getEntity(0)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(2)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(3)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(4)->getStepSpeed(), 1e-5);

	// update time to 1 second (6)
	// one step to apply it (during ticking step)
	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// ent2 & ent3 should be buffed
	EXPECT_NEAR(4., state_l->getEntity(1)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(4., state_l->getEntity(2)->getStepSpeed(), 1e-5);
	// not buffed
	EXPECT_NEAR(1., state_l->getEntity(0)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(3)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(4)->getStepSpeed(), 1e-5);

	// update time to 4 second (10)
	controller_l.update(4.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// ent2 & ent3 should be buffed
	EXPECT_NEAR(4., state_l->getEntity(1)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(4., state_l->getEntity(2)->getStepSpeed(), 1e-5);
	// not buffed
	EXPECT_NEAR(1., state_l->getEntity(0)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(3)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(4)->getStepSpeed(), 1e-5);

	// update time to 20 second (30)
	controller_l.update(20.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// ent2 & ent3 should be buffed (reload is enough)
	EXPECT_NEAR(4., state_l->getEntity(1)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(4., state_l->getEntity(2)->getStepSpeed(), 1e-5);
	// not buffed
	EXPECT_NEAR(1., state_l->getEntity(0)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(3)->getStepSpeed(), 1e-5);
	EXPECT_NEAR(1., state_l->getEntity(4)->getStepSpeed(), 1e-5);
}
