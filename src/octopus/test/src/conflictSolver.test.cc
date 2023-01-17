#include <gtest/gtest.h>

#include "step/ConflictPositionSolver.hh"
#include "step/Step.hh"
#include "step/entity/EntityMoveStep.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

using namespace octopus;

TEST(conflictSolverTest, simple)
{
	State state_l;
	Step step_l;

	Entity * ent_l = new Entity { { 6., 3. }, 1., false} ;
	Entity * building_l = new Entity { { 4., 2. }, 1., true} ;
	building_l->_isBuilding = true;

	state_l.addEntity(ent_l);
	state_l.addEntity(building_l);

	updateGrid(state_l, ent_l, true);
	updateGrid(state_l, building_l, true);

	EntityMoveStep * moveStep_l = new EntityMoveStep(0, Vector { -1, -1} );
	step_l.addEntityMoveStep(moveStep_l);

	updateStepFromConflictPosition(step_l, state_l);

	EXPECT_EQ(0., moveStep_l->_move.x);
	EXPECT_EQ(-1., moveStep_l->_move.y);

}
