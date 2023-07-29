#include <gtest/gtest.h>

#include "step/ConflictPositionSolver.hh"
#include "step/Step.hh"
#include "step/entity/EntityMoveStep.hh"
#include "state/State.hh"
#include "state/player/Player.hh"
#include "utils/Fixed.hh"
#include "logger/Logger.hh"

using namespace octopus;

TEST(conflictSolverTest, simple)
{
	State state_l;
	Step step_l(nullptr);

	state_l.getPlayers().push_back(new Player());

	octopus::EntityModel unitModel_l { false, 1., 1., 10. };
	octopus::EntityModel buildingModel_l { true, 1., 1., 10. };

	Entity * ent_l = new Entity { { 6., 3. }, false, unitModel_l} ;
	Entity * building_l = new Entity { { 4., 2. }, true, buildingModel_l} ;

	state_l.addEntity(ent_l);
	state_l.addEntity(building_l);

	updateGrid(state_l, ent_l, true);
	updateVisionGrid(state_l, ent_l, true);
	updateGrid(state_l, building_l, true);
	updateVisionGrid(state_l, building_l, true);

	EntityMoveStep * moveStep_l = new EntityMoveStep(Handle(0), Vector { -1, -1} );
	step_l.addEntityMoveStep(moveStep_l);

	updateStepFromConflictPosition(step_l, state_l);

	EXPECT_EQ(0., to_double(moveStep_l->_move.x));
	EXPECT_EQ(-1., to_double(moveStep_l->_move.y));

}
