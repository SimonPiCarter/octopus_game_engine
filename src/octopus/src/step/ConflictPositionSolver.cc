#include "ConflictPositionSolver.hh"

#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <cmath>
#include <unordered_set>

#include "state/entity/Entity.hh"
#include "state/State.hh"
#include "step/entity/EntityMoveStep.hh"
#include "step/Step.hh"
#include "utils/Vector.hh"

namespace octopus
{

struct Box
{
	long _lowerX {0};
	long _upperX {0};
	long _lowerY {0};
	long _upperY {0};
};

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
	/// map to access move step from entity
	std::unordered_map<Entity const *, EntityMoveStep *> mapMoveStep_l;
	std::vector<Vector> newPos_l(state_p.getEntities().size());

	for(EntityMoveStep *step_l: step_p.getEntityMoveStep())
	{
		Entity const *ent_l = state_p.getEntity(step_l->_handle);
		mapMoveStep_l[ent_l] = step_l;
		newPos_l[ent_l->_handle] = ent_l->_pos + step_l->_move;
	}

	// grid for fast access
	long size_l = 500;
	static std::vector<std::vector<std::list<EntityMoveStep *> > > grid_l;
	if(grid_l.empty())
	{
		grid_l.reserve(size_l);
		for(size_t i = 0 ; i < size_l ; ++ i)
		{
			grid_l.emplace_back(size_l);
		}
	}
	else
	{
		for(size_t i = 0 ; i < size_l ; ++ i)
		{
			for(size_t j = 0 ; j < size_l ; ++ j)
			{
				grid_l[i][j].clear();
			}
		}
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

		// fill grid
		Box box_l { long(newPos_l[ent_l->_handle].x-ent_l->_ray),
				   long(newPos_l[ent_l->_handle].x+ent_l->_ray+0.999),
				   long(newPos_l[ent_l->_handle].y-ent_l->_ray),
				   long(newPos_l[ent_l->_handle].y+ent_l->_ray+0.999) };
		for(size_t x = box_l._lowerX+size_l/2 ; x < box_l._upperX+size_l/2; ++x)
		{
			for(size_t y = box_l._lowerY+size_l/2 ; y < box_l._upperY+size_l/2; ++y)
			{
				grid_l[x][y].push_back(mapMoveStep_l[ent_l]);
			}
		}
	}

	/// map to access correction from step
	std::unordered_map<EntityMoveStep *, Vector> mapCorrection_l;
	// check every entity with one another
	for(EntityMoveStep *stepA_l: step_p.getEntityMoveStep())
	{
		Entity const * entA_l = state_p.getEntity(stepA_l->_handle);
		Box box_l {long(newPos_l[entA_l->_handle].x-entA_l->_ray),
				  long(newPos_l[entA_l->_handle].x+entA_l->_ray+0.999),
				  long(newPos_l[entA_l->_handle].y-entA_l->_ray),
				  long(newPos_l[entA_l->_handle].y+entA_l->_ray+0.999)};

		std::unordered_set<EntityMoveStep *, HandleHash> set_l;
		for(size_t x = box_l._lowerX+size_l/2 ; x < box_l._upperX+size_l/2; ++x)
		{
			for(size_t y = box_l._lowerY+size_l/2 ; y < box_l._upperY+size_l/2; ++y)
			{
				set_l.insert(grid_l[x][y].begin(), grid_l[x][y].end());
			}
		}

		// check every entity with one another
		for(EntityMoveStep *stepB_l : set_l)
		{
			// break if same
			if(stepB_l == stepA_l)
			{
				break;
			}
			Entity const * entB_l = state_p.getEntity(stepB_l->_handle);
			// check collision
			if(collision(newPos_l[entA_l->_handle], newPos_l[entB_l->_handle], entA_l->_ray, entB_l->_ray))
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
		double square_l = square_length(ent_l->_move);
		ent_l->_move = ent_l->_move + mapCorrection_l[ent_l] * 0.9;
		double newSquare_l = square_length(ent_l->_move);
		if(newSquare_l > square_l)
		{
			ent_l->_move = ent_l->_move * std::sqrt(square_l/newSquare_l);
		}
	}

	return updated_l;
}

} // namespace octopus

