#include <gtest/gtest.h>

#include "step/ConstraintPosition.hh"
#include "step/Step.hh"
#include "step/entity/EntityMoveStep.hh"
#include "state/State.hh"
#include "state/player/Player.hh"
#include "utils/Fixed.hh"

using namespace octopus;

TEST(constraintPositionTest, no_constraint)
{
	State state_l;
	Step step_l(nullptr);

	state_l.getPlayers().push_back(new Player());

	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	Entity * ent_l = new Entity { { 6., 3. }, false, unitModel_l} ;

	state_l.addEntity(ent_l);

	EntityMoveStep * moveStep_l = new EntityMoveStep(0, Vector { -1, -1} );
	step_l.addEntityMoveStep(moveStep_l);

	ConstraintPositionLibrary constraintLib_l;

	updateStepFromConstraintPosition(step_l, state_l, constraintLib_l);

	EXPECT_NEAR(-1., to_double(moveStep_l->_move.x), 1e-3);
	EXPECT_NEAR(-1., to_double(moveStep_l->_move.y), 1e-3);
}

///
/// X Constraint
///

TEST(constraintPositionTest, constraint_x_less)
{
	State state_l;
	Step step_l(nullptr);

	state_l.getPlayers().push_back(new Player());

	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	Entity * ent_l = new Entity { { 6., 3. }, false, unitModel_l} ;

	state_l.addEntity(ent_l);

	EntityMoveStep * moveStep_l = new EntityMoveStep(0, Vector { -1, -1} );
	step_l.addEntityMoveStep(moveStep_l);

	ConstraintPositionLibrary constraintLib_l;
	ConstraintPositionX cstr_l {-2.,-5.,5.,true};
	constraintLib_l.getXConstraintForPlayer(0).push_back(cstr_l);

	updateStepFromConstraintPosition(step_l, state_l, constraintLib_l);

	EXPECT_NEAR(-8., to_double(moveStep_l->_move.x), 1e-3);
	EXPECT_NEAR(-1., to_double(moveStep_l->_move.y), 1e-3);
}

TEST(constraintPositionTest, constraint_x_more)
{
	State state_l;
	Step step_l(nullptr);

	state_l.getPlayers().push_back(new Player());

	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	Entity * ent_l = new Entity { { 6., 3. }, false, unitModel_l} ;

	state_l.addEntity(ent_l);

	EntityMoveStep * moveStep_l = new EntityMoveStep(0, Vector { -1, -1} );
	step_l.addEntityMoveStep(moveStep_l);

	ConstraintPositionLibrary constraintLib_l;
	ConstraintPositionX cstr_l {7.,-5.,5.,false};
	constraintLib_l.getXConstraintForPlayer(0).push_back(cstr_l);

	updateStepFromConstraintPosition(step_l, state_l, constraintLib_l);

	EXPECT_NEAR(1., to_double(moveStep_l->_move.x), 1e-3);
	EXPECT_NEAR(-1., to_double(moveStep_l->_move.y), 1e-3);
}

TEST(constraintPositionTest, constraint_x_more_not_applying)
{
	State state_l;
	Step step_l(nullptr);

	state_l.getPlayers().push_back(new Player());

	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	Entity * ent_l = new Entity { { 6., 3. }, false, unitModel_l} ;

	state_l.addEntity(ent_l);

	EntityMoveStep * moveStep_l = new EntityMoveStep(0, Vector { -1, -1} );
	step_l.addEntityMoveStep(moveStep_l);

	ConstraintPositionLibrary constraintLib_l;
	ConstraintPositionX cstr_l {7.,4.,5.,false};
	constraintLib_l.getXConstraintForPlayer(0).push_back(cstr_l);

	updateStepFromConstraintPosition(step_l, state_l, constraintLib_l);

	EXPECT_NEAR(-1., to_double(moveStep_l->_move.x), 1e-3);
	EXPECT_NEAR(-1., to_double(moveStep_l->_move.y), 1e-3);
}

///
/// Y Constraint
///

TEST(constraintPositionTest, constraint_y_less)
{
	State state_l;
	Step step_l(nullptr);

	state_l.getPlayers().push_back(new Player());

	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	Entity * ent_l = new Entity { { 6., 3. }, false, unitModel_l} ;

	state_l.addEntity(ent_l);

	EntityMoveStep * moveStep_l = new EntityMoveStep(0, Vector { -1, -1} );
	step_l.addEntityMoveStep(moveStep_l);

	ConstraintPositionLibrary constraintLib_l;
	ConstraintPositionY cstr_l {-2.,-5.,5.,true};
	constraintLib_l.getYConstraintForPlayer(0).push_back(cstr_l);

	updateStepFromConstraintPosition(step_l, state_l, constraintLib_l);

	EXPECT_NEAR(-1., to_double(moveStep_l->_move.x), 1e-3);
	EXPECT_NEAR(-5., to_double(moveStep_l->_move.y), 1e-3);
}

TEST(constraintPositionTest, constraint_y_more)
{
	State state_l;
	Step step_l(nullptr);

	state_l.getPlayers().push_back(new Player());

	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	Entity * ent_l = new Entity { { 6., 3. }, false, unitModel_l} ;

	state_l.addEntity(ent_l);

	EntityMoveStep * moveStep_l = new EntityMoveStep(0, Vector { -1, -1} );
	step_l.addEntityMoveStep(moveStep_l);

	ConstraintPositionLibrary constraintLib_l;
	ConstraintPositionY cstr_l {5.,-5.,5.,false};
	constraintLib_l.getYConstraintForPlayer(0).push_back(cstr_l);

	updateStepFromConstraintPosition(step_l, state_l, constraintLib_l);

	EXPECT_NEAR(-1., to_double(moveStep_l->_move.x), 1e-3);
	EXPECT_NEAR(2., to_double(moveStep_l->_move.y), 1e-3);
}

TEST(constraintPositionTest, constraint_y_more_not_applying)
{
	State state_l;
	Step step_l(nullptr);

	state_l.getPlayers().push_back(new Player());

	octopus::EntityModel unitModel_l { false, 1., 1., 10. };

	Entity * ent_l = new Entity { { 6., 3. }, false, unitModel_l} ;

	state_l.addEntity(ent_l);

	EntityMoveStep * moveStep_l = new EntityMoveStep(0, Vector { -1, -1} );
	step_l.addEntityMoveStep(moveStep_l);

	ConstraintPositionLibrary constraintLib_l;
	ConstraintPositionY cstr_l {5.,-5.,0.,false};
	constraintLib_l.getYConstraintForPlayer(0).push_back(cstr_l);

	updateStepFromConstraintPosition(step_l, state_l, constraintLib_l);

	EXPECT_NEAR(-1., to_double(moveStep_l->_move.x), 1e-3);
	EXPECT_NEAR(-1., to_double(moveStep_l->_move.y), 1e-3);
}
