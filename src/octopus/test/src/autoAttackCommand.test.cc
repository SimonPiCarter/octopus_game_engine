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

TEST(autoAttackCommandTest, simple)
{
	UnitModel unitModel1_l { false, 1., 1., 10. };
	unitModel1_l._damage = 6.;
	unitModel1_l._range = 1.;
	UnitModel unitModel2_l { false, 1., 1., 10. };
	unitModel2_l._damage = 0.;
	unitModel2_l._range = 1.;

	Unit ent1_l { { 3, 3. }, false, unitModel1_l};
	ent1_l._player = 0;

	Unit ent2_l { { 8, 3. }, false, unitModel2_l};
	ent2_l._player = 1;

	Unit ent3_l { { 5, 8. }, false, unitModel2_l};
	ent3_l._player = 1;

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new UnitSpawnStep(Handle(0), ent1_l),
		new UnitSpawnStep(Handle(1), ent2_l),
		new UnitSpawnStep(Handle(2), ent3_l)
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

	// wind up should just be over but no damage still
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);

	// Next damage should be -> reload time + windup
	// 10 + 3 (13)

	controller_l.update(12.); // (19)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// damage has been done
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);

	controller_l.update(1.); // (20)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// damage has been done twice
	EXPECT_NEAR(0., to_double(state_l->getEntity(Handle(1))->_hp), 1e-5);

	controller_l.update(3.); // (23)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(4., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

	controller_l.update(1.); // (24)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);

	// Next damage should be -> reload time + windup
	// 10 + 3 (13) after last attack (happened on time step 19)
	// next damage should be on time step 32

	controller_l.update(8.); // (32)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(0))->_pos.x), 1e-5);
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(0))->_pos.y), 1e-5);
	// wind up should just be over but no damage still
	EXPECT_NEAR(10., to_double(state_l->getEntity(Handle(2))->_hp), 1e-5);

	controller_l.update(1.); // (33)
	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();

	// damage should be done
	EXPECT_NEAR(5., to_double(state_l->getEntity(Handle(2))->_hp), 1e-5);
}
