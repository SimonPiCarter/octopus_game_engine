#include <gtest/gtest.h>

#include <command/entity/EntityFlockMoveCommand.hh>
#include <controller/Controller.hh>
#include <step/entity/EntityMoveStep.hh>
#include <step/entity/spawn/EntitySpawnStep.hh>
#include <step/command/CommandQueueStep.hh>
#include <step/player/PlayerSpawnStep.hh>
#include <state/State.hh>

///
/// This test suite aims at checking that EntityFlockMoveCommand works properly
/// - Move an entity to the target
/// - Attack the target then terminate
///

using namespace octopus;

TEST(entityFlockCommandTest, test_data)
{
	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	Controller controller_l({
		new PlayerSpawnStep(0, 0),
		new EntitySpawnStep(Handle(0), Entity { { 3, 3. }, false, unitModel_l}),
		new EntitySpawnStep(Handle(1), Entity { { 5, 3. }, false, unitModel_l})
	}, 1);

	// query state
	State const * state_l = controller_l.queryState();

	controller_l.update(1);		// (1)

	// updated until synced up
	while(!controller_l.loop_body()) {}

	EntityFlockMoveCommand * cmd_l = new EntityFlockMoveCommand({Handle(0), Handle(1)}, { 10, 3. });
	controller_l.commitCommand(cmd_l);

	controller_l.update(1);		// (1)

	// updated until synced up
	while(!controller_l.loop_body()) {}

	state_l = controller_l.queryState();
	Entity const *ent_l = state_l->getEntity(Handle(0));
	MoveData const * data_l = dynamic_cast<MoveData const*>(getData(ent_l->getQueue().getFrontCommand()._var));

	ASSERT_NE(nullptr, data_l);
	ASSERT_NE(nullptr, data_l->_flockInfo);
	EXPECT_EQ(0l, data_l->_flockInfo->at(0).qtyReached);
	EXPECT_EQ(0l, data_l->_flockInfo->at(1).qtyReached);
	EXPECT_EQ(0l, data_l->_flockInfo->at(2).qtyReached);

	controller_l.update(20);	// (22)

	// updated until synced up
	while(!controller_l.loop_body()) {}

	// locked state should not have been updated
	EXPECT_EQ((0 == state_l->_id?0l: 2l), data_l->_flockInfo->at(0).qtyReached);
	EXPECT_EQ((1 == state_l->_id?0l: 2l), data_l->_flockInfo->at(1).qtyReached);
	EXPECT_EQ((2 == state_l->_id?0l: 2l), data_l->_flockInfo->at(2).qtyReached);

	// check command data
	EXPECT_EQ((0 == state_l->_id?0l: 2l), cmd_l->getInfo().at(0).qtyReached);
	EXPECT_EQ((1 == state_l->_id?0l: 2l), cmd_l->getInfo().at(1).qtyReached);
	EXPECT_EQ((2 == state_l->_id?0l: 2l), cmd_l->getInfo().at(2).qtyReached);

	// updated all states
	state_l = controller_l.queryState();
	while(!controller_l.loop_body()) {}

	// check command data
	EXPECT_EQ(2l, cmd_l->getInfo().at(0).qtyReached);
	EXPECT_EQ(2l, cmd_l->getInfo().at(1).qtyReached);
	EXPECT_EQ(2l, cmd_l->getInfo().at(2).qtyReached);
}
