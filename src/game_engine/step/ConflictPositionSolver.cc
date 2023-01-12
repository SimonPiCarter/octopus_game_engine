#include "ConflictPositionSolver.hh"

#include <stdexcept>
#include <unordered_map>
#include <iostream>

#include "entity/EntityStep.hh"
#include "entity/Vector.hh"
#include "step/Step.hh"

namespace game_engine
{
bool collision(EntityStep const *entA_p, EntityStep const *entB_p)
{
	// repulsion against axis between both (from B to A)
	Vector axis_l = entA_p->_newPosition - entB_p->_newPosition;

	// lenght between positions
	double squareLength_l = square_length(axis_l);
	double squareRay_l = (entA_p->_ent->_ray + entB_p->_ent->_ray)*(entA_p->_ent->_ray + entB_p->_ent->_ray);

	return squareLength_l < squareRay_l;
}

void updateStepFromConflictPosition(Step &step_p)
{
	std::unordered_map<EntityStep *, Vector> mapCorrection_l;
	// check every entity with one another
	for(EntityStep *entA_l: step_p._vecEntityStep)
	{
		// check every entity with one another
		for(EntityStep *entB_l : step_p._vecEntityStep)
		{
			// break if same
			if(entB_l == entA_l)
			{
				break;
			}
			// check collision
			if(collision(entA_l, entB_l))
			{
				// repulsion against axis between both (from B to A)
				Vector axis_l = entA_l->_newPosition - entB_l->_newPosition;

				// lenght between positions
				double length_l = length(axis_l);

				// collision distance (distance missing to avoid collision)
				double distance_l = entA_l->_ent->_ray + entB_l->_ent->_ray - length_l;
				if(distance_l < 0.)
				{
					throw std::logic_error("game_engine :: Error collision but no distance to fix");
				}

				Vector normalizedAxis_l { 0., 1.};
				if(length_l > 1e-3)
				{
					// Normalized axis from B to A
					normalizedAxis_l = axis_l / length_l;
				}

				double coefA_l = 0.5;
				double coefB_l = 0.5;
				if(entA_l->_ent->_frozen && entB_l->_ent->_frozen)
				{
					coefA_l = 0.;
					coefB_l = 0.;
				}
				if(entA_l->_ent->_frozen)
				{
					coefA_l = 0.;
					coefB_l = 1.;
				}
				else if(entB_l->_ent->_frozen)
				{
					coefA_l = 1.;
					coefB_l = 0.;
				}
				// updated steps, both doing half distance
				mapCorrection_l[entA_l] += normalizedAxis_l * distance_l * coefA_l;
				mapCorrection_l[entB_l] -= normalizedAxis_l * distance_l * coefB_l;
			}
		}
	}
	for(EntityStep *ent_l: step_p._vecEntityStep)
	{
		ent_l->_newPosition = ent_l->_newPosition + mapCorrection_l[ent_l] * 0.9;
	}
}

} // namespace game_engine

