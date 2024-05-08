#include <gtest/gtest.h>

#include <logger/Logger.hh>

#include <command/entity/EntityAttackCommand.hh>
#include <controller/Controller.hh>
#include <step/entity/EntityMoveStep.hh>
#include <step/entity/spawn/UnitSpawnStep.hh>
#include <step/command/CommandQueueStep.hh>
#include <step/player/PlayerSpawnStep.hh>
#include <state/State.hh>
#include <state/model/entity/UnitModel.hh>

///
/// This test suite aims at checking that EntityAttackCommand works properly
/// - Move an entity to the target
/// - Attack the target then terminate
///

using namespace octopus;

TEST(autoAttackChainingCommandTest, no_range)
{
	UnitModel unitModel1_l { false, 1., 1., 10. };
	unitModel1_l._damage = 6.;
	unitModel1_l._range = 1.;
	unitModel1_l._defaultAttackMod = ChainingModifier(1, 4, 0.5, 10);
	UnitModel unitModel2_l { false, 1., 1., 10. };
	unitModel2_l._damage = 0.;
	unitModel2_l._range = 1.;

	Unit ent1_l { { 3, 3. }, false, unitModel1_l};
	ent1_l._player = 0;

	Unit ent2_l { { 8, 3. }, false, unitModel2_l};
	ent2_l._player = 1;

	Unit ent3_l { { 15, 3. }, false, unitModel2_l};
	ent3_l._player = 1;

	Unit ent4_l { { 20, 3. }, false, unitModel2_l};
	ent4_l._player = 1;

	Unit ent5_l { { 31, 3. }, false, unitModel2_l};
	ent5_l._player = 1;

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new UnitSpawnStep(Handle(0), ent1_l),
		new UnitSpawnStep(Handle(1), ent2_l),
		new UnitSpawnStep(Handle(2), ent3_l),
		new UnitSpawnStep(Handle(3), ent4_l),
		new UnitSpawnStep(Handle(4), ent5_l)
	}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

	controller_l.update(3.); // (4)

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

	controller_l.update(2.); // (6)

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);

	controller_l.update(1.); // (7)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// wind up should just be over and damage done
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);
	// chaining has not proc'ed yet
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(2))->_hp), 1e-5);
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(3))->_hp), 1e-5);
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(4))->_hp), 1e-5);

	// chaning has a ticktime of 1 which means that we need 1 step to add the ticking and 1 step to complete it

	controller_l.update(2.); // (9)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// wind up should just be over and damage done
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);
	EXPECT_NEAR(7., to_double(state_l->getEntity(Handle(2))->_hp), 1e-5);
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(3))->_hp), 1e-5);
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(4))->_hp), 1e-5);

	// chaning has a ticktime of 1 which means that we need 1 step to add the ticking and 1 step to complete it

	controller_l.update(2.); // (11)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// wind up should just be over and damage done
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);
	EXPECT_NEAR(7., to_double(state_l->getEntity(Handle(2))->_hp), 1e-5);
	EXPECT_NEAR(8.5, to_double(state_l->getEntity(Handle(3))->_hp), 1e-5);
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(4))->_hp), 1e-5);

	// chaning has a ticktime of 1 which means that we need 1 step to add the ticking and 1 step to complete it

	controller_l.update(2.); // (13)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// wind up should just be over and damage done
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);
	EXPECT_NEAR(7., to_double(state_l->getEntity(Handle(2))->_hp), 1e-5);
	EXPECT_NEAR(8.5, to_double(state_l->getEntity(Handle(3))->_hp), 1e-5);
	// out of range
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(4))->_hp), 1e-5);
}

TEST(autoAttackChainingCommandTest, no_more_tick)
{
	UnitModel unitModel1_l { false, 1., 1., 10. };
	unitModel1_l._damage = 6.;
	unitModel1_l._range = 1.;
	unitModel1_l._defaultAttackMod = ChainingModifier(1, 2, 0.5, 100);
	UnitModel unitModel2_l { false, 1., 1., 10. };
	unitModel2_l._damage = 0.;
	unitModel2_l._range = 1.;

	Unit ent1_l { { 3, 3. }, false, unitModel1_l};
	ent1_l._player = 0;

	Unit ent2_l { { 8, 3. }, false, unitModel2_l};
	ent2_l._player = 1;

	Unit ent3_l { { 15, 3. }, false, unitModel2_l};
	ent3_l._player = 1;

	Unit ent4_l { { 20, 3. }, false, unitModel2_l};
	ent4_l._player = 1;

	Unit ent5_l { { 31, 3. }, false, unitModel2_l};
	ent5_l._player = 1;

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new UnitSpawnStep(Handle(0), ent1_l),
		new UnitSpawnStep(Handle(1), ent2_l),
		new UnitSpawnStep(Handle(2), ent3_l),
		new UnitSpawnStep(Handle(3), ent4_l),
		new UnitSpawnStep(Handle(4), ent5_l)
	}, 1.);

	// query state
	State const * state_l = controller_l.queryState();

	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

	controller_l.update(1.);

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

	controller_l.update(3.); // (4)

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

	controller_l.update(2.); // (6)

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(3., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);

	controller_l.update(1.); // (7)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// wind up should just be over and damage done
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);
	// chaining has not proc'ed yet
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(2))->_hp), 1e-5);
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(3))->_hp), 1e-5);
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(4))->_hp), 1e-5);

	// chaning has a ticktime of 1 which means that we need 1 step to add the ticking and 1 step to complete it

	controller_l.update(2.); // (9)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// wind up should just be over and damage done
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);
	EXPECT_NEAR(7., to_double(state_l->getEntity(Handle(2))->_hp), 1e-5);
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(3))->_hp), 1e-5);
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(4))->_hp), 1e-5);

	// chaning has a ticktime of 1 which means that we need 1 step to add the ticking and 1 step to complete it

	controller_l.update(2.); // (11)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// wind up should just be over and damage done
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);
	EXPECT_NEAR(7., to_double(state_l->getEntity(Handle(2))->_hp), 1e-5);
	EXPECT_NEAR(8.5, to_double(state_l->getEntity(Handle(3))->_hp), 1e-5);
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(4))->_hp), 1e-5);

	// chaning has a ticktime of 1 which means that we need 1 step to add the ticking and 1 step to complete it

	controller_l.update(2.); // (13)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// wind up should just be over and damage done
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);
	EXPECT_NEAR(7., to_double(state_l->getEntity(Handle(2))->_hp), 1e-5);
	EXPECT_NEAR(8.5, to_double(state_l->getEntity(Handle(3))->_hp), 1e-5);
	// out of ticks
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(4))->_hp), 1e-5);
}
