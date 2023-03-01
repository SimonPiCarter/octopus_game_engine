#include <gtest/gtest.h>

#include "command/CommandData.hh"
#include "command/data/MoveData.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "graph/PathManager.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "step/entity/EntityMoveStep.hh"
#include "step/Step.hh"
#include "utils/Vector.hh"

///
/// This test suite aims at checking that EntityMoveCommand works properly
/// - Generate the correct step
/// - terminates correctly
/// - goes through waypoints correctly
///

using namespace octopus;

///
/// Create entity on 3,3
/// Create move commande with way point 4,3 -> 4,4
///
TEST(moveCommandTest, simple)
{
	State state_l;
	state_l.getPlayers().push_back(new Player());

	PathManager pathManager_l;
	pathManager_l.initFromGrid(state_l.getPathGrid().getInternalGrid(), 1);
	pathManager_l.queryFlowField(4, 3);
	pathManager_l.compute(500);

	octopus::EntityModel unitModel_l { false, 0.9, 1., 10. };
	state_l.addEntity(new Entity { { 3, 3. }, false, unitModel_l});

	EntityMoveCommand command_l(0, 0, {4, 3}, 0, { {4, 3}});

	state_l.getEntity(0)->enqueue(&command_l, false);
	MoveData *data_l = dynamic_cast<MoveData *>(state_l.getEntity(0)->getFrontQueue()._data);
	EXPECT_NE(nullptr, data_l);

	///
	/// Step 1
	///
	Step step_l;

	bool terminated_l = command_l.applyCommand(step_l, state_l, data_l, pathManager_l);
	EXPECT_FALSE(terminated_l);

	ASSERT_EQ(1u, step_l.getEntityMoveStep().size());

	EntityMoveStep * entStep_l = *step_l.getEntityMoveStep().begin();

	Vector expected_l {1., 0};
	EXPECT_TRUE(expected_l == entStep_l->_move) << "expected : "<<expected_l<<" real : "<<entStep_l->_move;

	apply(step_l, state_l);

	// Now on 4,3

	EXPECT_NEAR(4., state_l.getEntity(0)->_pos.x.to_double(), 1e-5);
	EXPECT_NEAR(3., state_l.getEntity(0)->_pos.y.to_double(), 1e-5);

	///
	/// Step 2
	///
	Step step2_l;

	terminated_l = command_l.applyCommand(step2_l, state_l, data_l, pathManager_l);
	EXPECT_TRUE(terminated_l);

	ASSERT_EQ(0u, step2_l.getEntityMoveStep().size());

	apply(step2_l, state_l);

	// Still on 4,3

	EXPECT_NEAR(4., state_l.getEntity(0)->_pos.x.to_double(), 1e-5);
	EXPECT_NEAR(3., state_l.getEntity(0)->_pos.y.to_double(), 1e-5);

	///
	/// Step 3
	///
	Step step3_l;

	terminated_l = command_l.applyCommand(step3_l, state_l, data_l, pathManager_l);
	EXPECT_TRUE(terminated_l);

	ASSERT_EQ(0u, step3_l.getEntityMoveStep().size());
}
