#include "ConflictPositionSolver.hh"

#include <stdexcept>
#include <unordered_map>
#include <iostream>

#include "state/entity/Entity.hh"
#include "state/State.hh"
#include "step/entity/EntityMoveStep.hh"
#include "step/Step.hh"
#include "utils/Vector.hh"

namespace octopus
{
bool collision(Entity const * entA_p, EntityMoveStep const *stepA_p, Entity const * entB_p, EntityMoveStep const *stepB_p)
{
	// repulsion against axis between both (from B to A)
	Vector axis_l = (entA_p->_pos + stepA_p->_move) - (entB_p->_pos + stepB_p->_move);

	// lenght between positions
	double squareLength_l = square_length(axis_l);
	double squareRay_l = (entA_p->_ray + entB_p->_ray)*(entA_p->_ray + entB_p->_ray);

	return squareLength_l < squareRay_l;
}

void updateStepFromConflictPosition(Step &step_p, State const &state_p)
{
	std::unordered_map<EntityMoveStep *, Vector> mapCorrection_l;
	// check every entity with one another
	for(EntityMoveStep *stepA_l: step_p.getEntityMoveStep())
	{
		// check every entity with one another
		for(EntityMoveStep *stepB_l : step_p.getEntityMoveStep())
		{
			// break if same
			if(stepB_l == stepA_l)
			{
				break;
			}
			Entity const * entA_l = state_p.getEntity(stepA_l->_handle);
			Entity const * entB_l = state_p.getEntity(stepB_l->_handle);
			// check collision
			if(collision(entA_l, stepA_l, entB_l, stepB_l))
			{
				// repulsion against axis between both (from B to A)
				Vector axis_l = (entA_l->_pos + stepA_l->_move) - (entB_l->_pos + stepB_l->_move);

				// lenght between positions
				double length_l = length(axis_l);

				// collision distance (distance missing to avoid collision)
				double distance_l = entA_l->_ray + entB_l->_ray - length_l;
				if(distance_l < 0.)
				{
					throw std::logic_error("octopus :: Error collision but no distance to fix");
				}

				Vector normalizedAxis_l { 0., 1.};
				if(length_l > 1e-3)
				{
					// Normalized axis from B to A
					normalizedAxis_l = axis_l / length_l;
				}

				double coefA_l = 0.5;
				double coefB_l = 0.5;
				if(entA_l->_frozen && entB_l->_frozen)
				{
					coefA_l = 0.;
					coefB_l = 0.;
				}
				if(entA_l->_frozen)
				{
					coefA_l = 0.;
					coefB_l = 1.;
				}
				else if(entB_l->_frozen)
				{
					coefA_l = 1.;
					coefB_l = 0.;
				}
				// updated steps, both doing half distance
				mapCorrection_l[stepA_l] += normalizedAxis_l * distance_l * coefA_l;
				mapCorrection_l[stepB_l] -= normalizedAxis_l * distance_l * coefB_l;
			}
		}
	}
	for(EntityMoveStep *ent_l: step_p.getEntityMoveStep())
	{
		ent_l->_move = ent_l->_move + mapCorrection_l[ent_l] * 0.9;
	}
}

} // namespace octopus

