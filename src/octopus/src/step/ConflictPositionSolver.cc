#include "ConflictPositionSolver.hh"

#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <cmath>
#include <random>
#include <unordered_set>

#include "logger/Logger.hh"
#include "state/entity/Entity.hh"
#include "state/State.hh"
#include "step/entity/EntityMoveStep.hh"
#include "step/Step.hh"
#include "utils/Vector.hh"
#include "utils/DynamicBitset.hh"
#include "utils/Box.hh"
#include "utils/RandomGen.hh"

namespace octopus
{

bool collision(Vector const &posA_p, Vector const &posB_p, double const &rayA_p, double const &rayB_p)
{
	double const ray_l = rayA_p + rayB_p;
	// try quick win
	if(posA_p.x - posB_p.x > ray_l
	|| posB_p.x - posA_p.x > ray_l
	|| posA_p.y - posB_p.y > ray_l
	|| posB_p.y - posA_p.y > ray_l)
	{
		return false;
	}

	// repulsion against axis between both (from B to A)
	Vector axis_l = posA_p - posB_p;

	// lenght between positions
	double squareLength_l = square_length(axis_l);
	double squareRay_l = ray_l * ray_l;

	return squareLength_l < squareRay_l;
}

struct HandleHash
{
	Handle operator()(EntityMoveStep const *step_p) const
	{
		return step_p->_handle;
	}
};

bool updateStepFromConflictPosition(Step &step_p, State const &state_p)
{
	RandomGen gen_l(42);

	Logger::getDebug() << " conflict solver :: start"<<std::endl;
	/// map to access move step from entity
	std::unordered_map<Entity const *, EntityMoveStep *> mapMoveStep_l;
	std::vector<Vector> newPos_l(state_p.getEntities().size());

	for(EntityMoveStep *step_l: step_p.getEntityMoveStep())
	{
		Entity const *ent_l = state_p.getEntity(step_l->_handle);
		mapMoveStep_l[ent_l] = step_l;
		newPos_l[ent_l->_handle] = ent_l->_pos + step_l->_move;
	}

	// fill up move steps when missing
	for(Entity const * ent_l : state_p.getEntities())
	{
		if(mapMoveStep_l[ent_l] == nullptr)
		{
			EntityMoveStep *step_l = new EntityMoveStep(ent_l->_handle, {0, 0});
			step_p.addEntityMoveStep(step_l);
			mapMoveStep_l[ent_l] = step_l;
			newPos_l[ent_l->_handle] = ent_l->_pos;
		}
	}

	/// map to access correction from step
	std::unordered_map<EntityMoveStep *, Vector> mapCorrection_l;
	/// this correction should not been normalized in the end (used with buildings)
	std::unordered_map<EntityMoveStep *, Vector> mapAbsoluteCorrection_l;

	//////////////////////////////
	// Set up bitset of collision
	//////////////////////////////

	// grid for fast access
	std::vector<std::vector<AbstractBitset *> > const & grid_l = state_p.getGrid();

	//////////////////////////////
	// Check on non Buildings
	//////////////////////////////

	// check every entity with one another
	for(EntityMoveStep *stepA_l: step_p.getEntityMoveStep())
	{
		Entity const * entA_l = state_p.getEntity(stepA_l->_handle);

		Box<long> box_l {state_p.getGridIndex(newPos_l[entA_l->_handle].x-entA_l->_model._ray),
						 state_p.getGridIndex(newPos_l[entA_l->_handle].x+entA_l->_model._ray+0.999),
						 state_p.getGridIndex(newPos_l[entA_l->_handle].y-entA_l->_model._ray),
						 state_p.getGridIndex(newPos_l[entA_l->_handle].y+entA_l->_model._ray+0.999)};
		std::vector<bool> bitset_l(state_p.getEntities().size(), false);
		for(size_t x = box_l._lowerX ; x <= box_l._upperX; ++x)
		{
		for(size_t y = box_l._lowerY ; y <= box_l._upperY; ++y)
		{
		// check every entity with one another
		grid_l[x][y]->for_each([&] (int handle_p)
		{
			if(bitset_l.at(handle_p))
			{
				return;
			}
			bitset_l[handle_p] = true;
			Entity const * entB_l = state_p.getEntity(handle_p);
			EntityMoveStep *stepB_l = mapMoveStep_l[entB_l];
			// continue if same
			if(stepB_l == stepA_l)
			{
				return;
			}
			if(entA_l->_model._isStatic || entB_l->_model._isStatic)
			{
				return;
			}
			// check collision
			else if(collision(newPos_l[entA_l->_handle], newPos_l[entB_l->_handle], entA_l->_model._ray, entB_l->_model._ray))
			{
				// repulsion against axis between both (from B to A)
				Vector axis_l = (entA_l->_pos + stepA_l->_move) - (entB_l->_pos + stepB_l->_move);

				// lenght between positions
				double length_l = length(axis_l);

				// collision distance (distance missing to avoid collision)
				double distance_l = entA_l->_model._ray + entB_l->_model._ray - length_l;
				if(distance_l < 0.)
				{
					throw std::logic_error("octopus :: Error collision but no distance to fix");
				}

				Vector normalizedAxis_l { 1., 0.};
				if(length_l > 1e-3)
				{
					// Normalized axis from B to A
					normalizedAxis_l = axis_l / length_l;
				}
				else
				{
					normalizedAxis_l = Vector { double(gen_l.nextFromRange(-10, 10)), double(gen_l.nextFromRange(-10, 10))};
					normalizedAxis_l = normalizedAxis_l / length(normalizedAxis_l);
				}
				double coefA_l = 0.5;
				if(square_length(stepA_l->_move) > 1e-5 || square_length(stepB_l->_move) > 1e-5)
				{
					coefA_l = std::sqrt(square_length(stepB_l->_move)/(square_length(stepA_l->_move)+square_length(stepB_l->_move)));
				}
				if(entA_l->isFrozen() && entB_l->isFrozen())
				{
					coefA_l = 0.;
				}
				if(entA_l->isFrozen())
				{
					coefA_l = 0.;
				}
				else if(entB_l->isFrozen())
				{
					coefA_l = 1.;
				}
				// updated steps, both doing half distance
				mapCorrection_l[stepA_l] += normalizedAxis_l * distance_l * coefA_l;
			}
		});
		}
		}
	}

	//////////////////////////////
	// Update from non building
	//////////////////////////////

	bool updated_l = false;
	for(EntityMoveStep *ent_l: step_p.getEntityMoveStep())
	{
		// if update
		if(mapCorrection_l[ent_l] != Vector {0,0})
		{
			updated_l = true;
		}
		else
		{
			continue;
		}
		// ensure that move does not become too cahotic
		double square_l = state_p.getEntity(ent_l->_handle)->getStepSpeed()*state_p.getEntity(ent_l->_handle)->getStepSpeed();
		Vector origMove_l = ent_l->_move;
		ent_l->_move = ent_l->_move + mapCorrection_l[ent_l] * 0.9;
		double newSquare_l = square_length(ent_l->_move);
		if(newSquare_l > square_l)
		{
			ent_l->_move = ent_l->_move * std::sqrt(square_l/newSquare_l);
		}
		// in case no move
		// if(newSquare_l < 1e-3)
		// {
		// 	// try a small perpendicular move to avoid two entities to block each other
		// 	Vector perpendicular_l {origMove_l.y, -origMove_l.x};
		// 	double rdmFactr_l = gen_l.nextFromRange(5, 20)/100.;
		// 	ent_l->_move += perpendicular_l * rdmFactr_l;
		// }

		Entity const *entA_l = state_p.getEntity(ent_l->_handle);
		newPos_l[entA_l->_handle] = entA_l->_pos + ent_l->_move;
	}

	//////////////////////////////
	// Check on Buildings
	//////////////////////////////

	// check every entity with one another on buildings
	for(EntityMoveStep *stepA_l: step_p.getEntityMoveStep())
	{
		Entity const * entA_l = state_p.getEntity(stepA_l->_handle);

		Box<long> box_l {state_p.getGridIndex(newPos_l[entA_l->_handle].x-entA_l->_model._ray),
						 state_p.getGridIndex(newPos_l[entA_l->_handle].x+entA_l->_model._ray+0.999),
						 state_p.getGridIndex(newPos_l[entA_l->_handle].y-entA_l->_model._ray),
						 state_p.getGridIndex(newPos_l[entA_l->_handle].y+entA_l->_model._ray+0.999)};

		std::vector<bool> bitset_l(state_p.getEntities().size(), false);
		for(size_t x = box_l._lowerX ; x <= box_l._upperX; ++x)
		{
		for(size_t y = box_l._lowerY ; y <= box_l._upperY; ++y)
		{
		// check every entity with one another
		grid_l[x][y]->for_each([&] (int handle_p)
		{
			if(bitset_l.at(handle_p))
			{
				return;
			}
			bitset_l[handle_p] = true;
			Entity const * entB_l = state_p.getEntity(handle_p);
			EntityMoveStep *stepB_l = mapMoveStep_l[entB_l];
			// continue if same
			if(stepB_l == stepA_l)
			{
				return;
			}
			if(!entA_l->_model._isStatic && !entB_l->_model._isStatic)
			{
				return;
			}
			// if one of the two is a building we check on rectangle instead of circles
			Box<double> boxA_l { newPos_l[entA_l->_handle].x - entA_l->_model._ray, newPos_l[entA_l->_handle].x + entA_l->_model._ray,
								newPos_l[entA_l->_handle].y - entA_l->_model._ray, newPos_l[entA_l->_handle].y + entA_l->_model._ray };
			Box<double> boxB_l { newPos_l[entB_l->_handle].x - entB_l->_model._ray, newPos_l[entB_l->_handle].x + entB_l->_model._ray,
								newPos_l[entB_l->_handle].y - entB_l->_model._ray, newPos_l[entB_l->_handle].y + entB_l->_model._ray };

			Box<double> intersect_l = { std::max(boxA_l._lowerX, boxB_l._lowerX),
								std::min(boxA_l._upperX, boxB_l._upperX),
								std::max(boxA_l._lowerY, boxB_l._lowerY),
								std::min(boxA_l._upperY, boxB_l._upperY) };
			// Check intersections
			if(intersect_l._lowerX < intersect_l._upperX
			&& intersect_l._lowerY < intersect_l._upperY)
			{
				// direction in regards of A
				double dXRight_l = boxA_l._upperX - boxB_l._lowerX;
				double dXLeft_l = boxA_l._lowerX - boxB_l._upperX;
				double dYUp_l = boxA_l._upperY - boxB_l._lowerY;
				double dYDown_l = boxA_l._lowerY - boxB_l._upperY;

				double dXRightAbs_l = std::abs(dXRight_l);
				double dXLeftAbs_l = std::abs(dXLeft_l);
				double dYUpAbs_l = std::abs(dYUp_l);
				double dYDownAbs_l = std::abs(dYDown_l);
				double min_l = std::min(dXRightAbs_l, std::min(dXLeftAbs_l, std::min(dYUpAbs_l, dYDownAbs_l)));
				// B -> A
				Vector diff_l { 0, 0 };
				if(min_l + 1e-5 >= dXRightAbs_l)
				{
					diff_l = {dXRight_l, 0.};
				} else if(min_l + 1e-5 >= dXLeftAbs_l) {
					diff_l = {dXLeft_l, 0.};
				} else if(min_l + 1e-5 >= dYUpAbs_l) {
					diff_l = {0., dYUp_l};
				} else if(min_l + 1e-5 >= dYDownAbs_l) {
					diff_l = {0., dYDown_l};
				}

				double coefA_l = 0.5;
				if(entA_l->isFrozen() && entB_l->isFrozen())
				{
					coefA_l = 0.;
				}
				if(entA_l->isFrozen())
				{
					coefA_l = 0.;
				}
				else if(entB_l->isFrozen())
				{
					coefA_l = 1.;
				}
				// updated steps, both doing half distance
				mapAbsoluteCorrection_l[stepA_l] -= diff_l * coefA_l;
			}
		});
		}
		}
	}

	//////////////////////////////
	// Update from building
	//////////////////////////////

	for(EntityMoveStep *ent_l: step_p.getEntityMoveStep())
	{
		// if update
		if(mapAbsoluteCorrection_l[ent_l] != Vector {0,0})
		{
			updated_l = true;
		}
		else
		{
			continue;
		}
		Logger::getDebug() << " conflict solver :: "<<ent_l->_handle<< " absolute correction : "<<mapAbsoluteCorrection_l[ent_l]<<std::endl;
		ent_l->_move = ent_l->_move + mapAbsoluteCorrection_l[ent_l];
	}

	return updated_l;
}

} // namespace octopus

