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

bool collision(Vector const &posA_p, Vector const &posB_p, Fixed const &rayA_p, Fixed const &rayB_p)
{
	Fixed const ray_l = rayA_p + rayB_p;
	// try quick win
	if(posA_p.x - posB_p.x > ray_l - 0.01
	|| posB_p.x - posA_p.x > ray_l - 0.01
	|| posA_p.y - posB_p.y > ray_l - 0.01
	|| posB_p.y - posA_p.y > ray_l - 0.01)
	{
		return false;
	}

	// repulsion against axis between both (from B to A)
	Vector axis_l = posA_p - posB_p;

	// lenght between positions
	Fixed squareLength_l = square_length(axis_l);
	Fixed squareRay_l = ray_l * ray_l;

	return squareLength_l < squareRay_l - 0.01;
}

struct HandleHash
{
	Handle operator()(EntityMoveStep const *step_p) const
	{
		return step_p->_handle;
	}
};

Fixed getRoot(Vector const &v, Vector const &d, Fixed sqR_p)
{
	// solve At^2 + Bt + C = 0
	Fixed A = v.x + v.y;
	Fixed B = 2 * d.x * v.x + 2 * d.y * v.y;
	Fixed C = d.x*d.x + d.y*d.y - sqR_p;

	Fixed delta = std::max(Fixed(0), B*B - 4*A*C);
	Fixed t = - C / B;
	if(A != 0)
	{
		Fixed sqrtDelta = numeric::square_root(delta);
		Fixed t1 = - B - sqrtDelta / (2*A);
		Fixed t2 = - B + sqrtDelta / (2*A);
		t = t1 > t2 ? t1 : t2;
	}

	if(t < 0)
	{
		//std::cout<<"neg?? "<<t<<std::endl;
		t = 0;
	}
	return t;
}

bool updateStepFromConflictPosition(Step &step_p, State const &state_p)
{
	RandomGen gen_l(42);

	Logger::getDebug() << " conflict solver :: start"<<std::endl;
	/// map to access move step from entity
	std::vector<EntityMoveStep *> mapMoveStep_l(state_p.getEntities().size(), nullptr);
	std::vector<Vector> newPos_l(state_p.getEntities().size());
	std::vector<Vector> prevMove_l(state_p.getEntities().size());

	for(EntityMoveStep *step_l: step_p.getEntityMoveStep())
	{
		Entity const *ent_l = state_p.getEntity(step_l->_handle);
		if(mapMoveStep_l[ent_l->_handle] != nullptr)
		{
			mapMoveStep_l[ent_l->_handle]->_move += step_l->_move;
			step_l->_move = Vector {0,0};
			newPos_l[ent_l->_handle] = ent_l->_pos + mapMoveStep_l[ent_l->_handle]->_move;
			Logger::getDebug() << " conflict solver :: added entity move step info [ent "<<ent_l->_handle<<"] to "<<newPos_l[ent_l->_handle]<<std::endl;
		}
		else
		{
			mapMoveStep_l[ent_l->_handle] = step_l;
			newPos_l[ent_l->_handle] = ent_l->_pos + step_l->_move;
			Logger::getDebug() << " conflict solver :: update entity move step info [ent "<<ent_l->_handle<<"] to "<<newPos_l[ent_l->_handle]<<std::endl;
		}
	}

	if(step_p.getPrev())
	{
		Logger::getDebug() << " conflict solver :: handling prev step"<<std::endl;
		for(EntityMoveStep *step_l : step_p.getPrev()->getEntityMoveStep())
		{
			Logger::getDebug() << " conflict solver :: adding [ent"<<step_l->_handle<<"] with move "<<step_l->_move<<std::endl;
			prevMove_l[step_l->_handle] = step_l->_move;
		}
	}

	// fill up move steps when missing
	for(Entity const * ent_l : state_p.getEntities())
	{
		if(mapMoveStep_l[ent_l->_handle] == nullptr && ent_l->isActive())
		{
			Logger::getDebug() << " conflict solver :: add entity move step because none "<<ent_l->_handle<<std::endl;
			EntityMoveStep *step_l = new EntityMoveStep(ent_l->_handle, {0, 0});
			step_p.addEntityMoveStep(step_l);
			mapMoveStep_l[ent_l->_handle] = step_l;
			newPos_l[ent_l->_handle] = ent_l->_pos;
		}
	}

	/// map to access correction from step
	std::vector<Vector> mapCorrection_l(state_p.getEntities().size());
	/// this correction should not been normalized in the end (used with buildings)
	std::vector<Vector> mapAbsoluteCorrection_l(state_p.getEntities().size());
	/// coef correction for movement
	std::vector<Fixed> mapDelta_l(state_p.getEntities().size(), 1);

	//////////////////////////////
	// Set up bitset of collision
	//////////////////////////////

	// grid for fast access
	std::vector<std::vector<AbstractBitset *> > const & grid_l = state_p.getGrid();

	//////////////////////////////
	// Check on non Buildings
	//////////////////////////////

	// check every entity with one another
	for(EntityMoveStep *stepA_l: mapMoveStep_l)
	{
		if(stepA_l == nullptr)
		{
			continue;
		}

		Entity const * entA_l = state_p.getEntity(stepA_l->_handle);

		if(entA_l->isIgnoringCollision())
		{
			continue;
		}

		/// limit the number of collison to avoid huge slow downs at start up
		size_t maxCol_l = 10;
		size_t nbCol_l = 0;
		Box<long> box_l {state_p.getGridIndex(newPos_l[entA_l->_handle].x-entA_l->_model._ray),
						 state_p.getGridIndex(newPos_l[entA_l->_handle].x+entA_l->_model._ray+0.999),
						 state_p.getGridIndex(newPos_l[entA_l->_handle].y-entA_l->_model._ray),
						 state_p.getGridIndex(newPos_l[entA_l->_handle].y+entA_l->_model._ray+0.999)};
		std::vector<char> bitset_l(state_p.getEntities().size(), 0);
		for(size_t x = box_l._lowerX ; x <= box_l._upperX; ++x)
		{
		for(size_t y = box_l._lowerY ; y <= box_l._upperY; ++y)
		{
		// check every entity with one another
		grid_l[x][y]->for_each([&] (int handle_p)
		{
			if(handle_p >= stepA_l->_handle)
			{
				return true;
			}
			if(maxCol_l <= nbCol_l)
			{
				return true;
			}
			if(bitset_l[handle_p] == 1)
			{
				return false;
			}
			bitset_l[handle_p] = 1;
			Entity const * entB_l = state_p.getEntity(handle_p);

			if(entB_l->isIgnoringCollision() || !entB_l->isActive())
			{
				return false;
			}

			EntityMoveStep *stepB_l = mapMoveStep_l[entB_l->_handle];
			if(entA_l->_model._isStatic || entB_l->_model._isStatic)
			{
				return false;
			}
			// check collision
			else
			{
				bool curCol_l = collision(entA_l->_pos, entB_l->_pos, entA_l->_model._ray, entB_l->_model._ray);
				bool colAtoB_l = collision(newPos_l[entA_l->_handle], entB_l->_pos, entA_l->_model._ray, entB_l->_model._ray);
				bool colBtoA_l = collision(newPos_l[entB_l->_handle], entA_l->_pos, entA_l->_model._ray, entB_l->_model._ray);

				Fixed curSqDist_l = square_length(entA_l->_pos - entB_l->_pos);
				Fixed nexSqDist_l = square_length(newPos_l[entA_l->_handle] - newPos_l[entB_l->_handle]);

				if(curCol_l && curSqDist_l >= nexSqDist_l)
				{
					++nbCol_l;
					// repulsion against axis between both (from B to A)
					Vector axis_l = (entA_l->_pos + stepA_l->_move) - (entB_l->_pos + stepB_l->_move);

					// lenght between positions
					Fixed length_l = length(axis_l);

					// collision distance (distance missing to avoid collision)
					Fixed distance_l = entA_l->_model._ray + entB_l->_model._ray - length_l;
					if(distance_l < 0.)
					{
						throw std::logic_error("octopus :: Error collision but no distance to fix");
					}

					Vector normalizedAxis_l { 1., 0.};
					if(axis_l.x > 1e-3 || axis_l.y > 1e-3)
					{
						// Normalized axis from B to A
						normalizedAxis_l = axis_l / length_l;
					}
					else
					{
						normalizedAxis_l = Vector { double(gen_l.nextFromRange(1, 10)), double(gen_l.nextFromRange(1, 10))};
						if(gen_l.nextFromRange(0, 1) > 0)
						{
							normalizedAxis_l.x = -normalizedAxis_l.x;
						}
						if(gen_l.nextFromRange(0, 1) > 0)
						{
							normalizedAxis_l.y = -normalizedAxis_l.y;
						}
						normalizedAxis_l = normalizedAxis_l / length(normalizedAxis_l);
					}
					Fixed coefA_l = 0.5;
					Fixed coefB_l = 0.5;
					if(square_length(stepA_l->_move) > 1e-5 || square_length(stepB_l->_move) > 1e-5)
					{
						coefA_l = numeric::square_root(square_length(stepB_l->_move)/(square_length(stepA_l->_move)+square_length(stepB_l->_move)));
						coefB_l = 1 - coefA_l;
					}
					if(entA_l->isFrozen() && entB_l->isFrozen())
					{
						coefA_l = 0.;
						coefB_l = 0.;
					}
					else if(entA_l->isFrozen())
					{
						coefA_l = 0.;
						coefB_l = 1.;
					}
					else if(entB_l->isFrozen())
					{
						coefA_l = 1.;
						coefB_l = 0.;
					}
					Logger::getDebug() << " conflict solver :: solving conflict [ent "<<stepA_l->_handle<<"] with "<<normalizedAxis_l * distance_l * coefA_l
									<< " and [ent "<<stepB_l->_handle<<"] with"<<normalizedAxis_l * distance_l * coefB_l * -1.<<std::endl;
					// updated steps, both doing half distance
					mapCorrection_l[stepA_l->_handle] += normalizedAxis_l * distance_l * coefA_l;
					mapCorrection_l[stepB_l->_handle] -= normalizedAxis_l * distance_l * coefB_l;
				}
				else
				{
					Fixed r_l = entA_l->_model._ray + entB_l->_model._ray;
					Fixed sqR_l = r_l * r_l;
					if(colAtoB_l && curSqDist_l > nexSqDist_l)
					{
						Fixed t = getRoot(stepA_l->_move, entA_l->_pos - entB_l->_pos, sqR_l);

						mapDelta_l[entA_l->_handle] = std::min(mapDelta_l[entA_l->_handle], t * 0.99);
					}
					if(colBtoA_l && curSqDist_l > nexSqDist_l)
					{
						Fixed t = getRoot(stepB_l->_move, entB_l->_pos - entA_l->_pos, sqR_l);

						mapDelta_l[entB_l->_handle] = std::min(mapDelta_l[entB_l->_handle], t * 0.99);
					}
				}


			}
			return false;
		});
		}
		}
	}

	//////////////////////////////
	// Update from non building
	//////////////////////////////

	bool updated_l = false;
	for(EntityMoveStep *entStep_l: step_p.getEntityMoveStep())
	{
		Entity const *ent_l = state_p.getEntity(entStep_l->_handle);
		// if update
		if(!is_zero(mapCorrection_l[ent_l->_handle]))
		{
			updated_l = true;
		}
		else
		{
			entStep_l->_move = entStep_l->_move * mapDelta_l[ent_l->_handle];
			newPos_l[ent_l->_handle] = ent_l->_pos + entStep_l->_move;
			continue;
		}
		// ensure that move does not become too chaotic
		Fixed square_l = state_p.getEntity(ent_l->_handle)->getStepSpeed()*state_p.getEntity(ent_l->_handle)->getStepSpeed();
		entStep_l->_move = mapCorrection_l[ent_l->_handle];
		Fixed newSquare_l = square_length(entStep_l->_move);
		if(newSquare_l > square_l)
		{
			entStep_l->_move = entStep_l->_move * numeric::square_root(square_l/newSquare_l);
		}
		// Vector origMove_l = entStep_l->_move;
		// in case no move
		// if(newSquare_l < 1e-3)
		// {
		// 	// try a small perpendicular move to avoid two entities to block each other
		// 	Vector perpendicular_l {origMove_l.y, -origMove_l.x};
		// 	double rdmFactr_l = gen_l.nextFromRange(5, 20)/100.;
		// 	entStep_l->_move += perpendicular_l * rdmFactr_l;
		// }

		newPos_l[ent_l->_handle] = ent_l->_pos + entStep_l->_move;
	}

	//////////////////////////////
	// Check on Buildings
	//////////////////////////////

	// check every entity with one another on buildings
	for(EntityMoveStep *stepA_l: step_p.getEntityMoveStep())
	{
		Entity const * entA_l = state_p.getEntity(stepA_l->_handle);

		if(entA_l->isIgnoringCollision())
		{
			continue;
		}

		Box<long> box_l {state_p.getGridIndex(newPos_l[entA_l->_handle].x-entA_l->_model._ray),
						 state_p.getGridIndex(newPos_l[entA_l->_handle].x+entA_l->_model._ray+0.999),
						 state_p.getGridIndex(newPos_l[entA_l->_handle].y-entA_l->_model._ray),
						 state_p.getGridIndex(newPos_l[entA_l->_handle].y+entA_l->_model._ray+0.999)};

		std::vector<char> bitset_l(state_p.getEntities().size(), 0);
		for(size_t x = box_l._lowerX ; x <= box_l._upperX; ++x)
		{
		for(size_t y = box_l._lowerY ; y <= box_l._upperY; ++y)
		{
		// check every entity with one another
		grid_l[x][y]->for_each([&] (int handle_p)
		{
			if(handle_p >= stepA_l->_handle)
			{
				return true;
			}
			if(bitset_l[handle_p] == 1)
			{
				return false;
			}
			bitset_l[handle_p] = 1;
			Entity const * entB_l = state_p.getEntity(handle_p);

			if(entB_l->isIgnoringCollision())
			{
				return false;
			}

			EntityMoveStep *stepB_l = mapMoveStep_l[entB_l->_handle];
			if(!entA_l->_model._isStatic && !entB_l->_model._isStatic)
			{
				return false;
			}
			// if one of the two is a building we check on rectangle instead of circles
			Box<Fixed> boxA_l { newPos_l[entA_l->_handle].x - entA_l->_model._ray, newPos_l[entA_l->_handle].x + entA_l->_model._ray,
								newPos_l[entA_l->_handle].y - entA_l->_model._ray, newPos_l[entA_l->_handle].y + entA_l->_model._ray };
			Box<Fixed> boxB_l { newPos_l[entB_l->_handle].x - entB_l->_model._ray, newPos_l[entB_l->_handle].x + entB_l->_model._ray,
								newPos_l[entB_l->_handle].y - entB_l->_model._ray, newPos_l[entB_l->_handle].y + entB_l->_model._ray };

			Box<Fixed> intersect_l = { std::max(boxA_l._lowerX, boxB_l._lowerX),
								std::min(boxA_l._upperX, boxB_l._upperX),
								std::max(boxA_l._lowerY, boxB_l._lowerY),
								std::min(boxA_l._upperY, boxB_l._upperY) };
			// Check intersections
			if(intersect_l._lowerX < intersect_l._upperX
			&& intersect_l._lowerY < intersect_l._upperY)
			{
				// direction in regards of A
				Fixed dXRight_l = boxA_l._upperX - boxB_l._lowerX;
				Fixed dXLeft_l = boxA_l._lowerX - boxB_l._upperX;
				Fixed dYUp_l = boxA_l._upperY - boxB_l._lowerY;
				Fixed dYDown_l = boxA_l._lowerY - boxB_l._upperY;

				Fixed dXRightAbs_l = numeric::abs(dXRight_l);
				Fixed dXLeftAbs_l = numeric::abs(dXLeft_l);
				Fixed dYUpAbs_l = numeric::abs(dYUp_l);
				Fixed dYDownAbs_l = numeric::abs(dYDown_l);
				Fixed min_l = std::min(dXRightAbs_l, std::min(dXLeftAbs_l, std::min(dYUpAbs_l, dYDownAbs_l)));
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
				double coefB_l = 0.5;
				if(entA_l->isFrozen() && entB_l->isFrozen())
				{
					coefA_l = 0.;
					coefB_l = 0.;
				}
				else if(entA_l->isFrozen())
				{
					coefA_l = 0.;
					coefB_l = 1.;
				}
				else if(entB_l->isFrozen())
				{
					coefA_l = 1.;
					coefB_l = 0.;
				}
				Logger::getDebug() << " conflict solver :: solving conflict [ent "<<stepA_l->_handle<<"] with "<<(diff_l * coefA_l * -1)
								   << " and [ent "<<stepB_l->_handle<<"] with"<<diff_l * coefB_l<<std::endl;
				// updated steps, both doing half distance
				mapAbsoluteCorrection_l[stepA_l->_handle] -= diff_l * coefA_l;
				mapAbsoluteCorrection_l[stepB_l->_handle] += diff_l * coefB_l;
			}
			return false;
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
		if(!is_zero(mapAbsoluteCorrection_l[ent_l->_handle]))
		{
			updated_l = true;
		}
		else
		{
			continue;
		}
		Logger::getDebug() << " conflict solver :: "<<ent_l->_handle<<" move "<<ent_l->_move<<" absolute correction : "<<mapAbsoluteCorrection_l[ent_l->_handle]<<std::endl;
		ent_l->_move = ent_l->_move + mapAbsoluteCorrection_l[ent_l->_handle];
	}


	//////////////////////////////
	// Update from map bounds
	//////////////////////////////

	for(EntityMoveStep *ent_l: step_p.getEntityMoveStep())
	{
		Entity const *entity_l = state_p.getEntity(ent_l->_handle);
		if(entity_l->_pos.x + ent_l->_move.x < 0)
		{
			updated_l = true;
			ent_l->_move.x = - entity_l->_pos.x + 1e-3 ;
		}
		else if(entity_l->_pos.x + ent_l->_move.x > state_p.getWorldSize())
		{
			updated_l = true;
			ent_l->_move.x = state_p.getWorldSize() - entity_l->_pos.x ;
		}
		if(entity_l->_pos.y + ent_l->_move.y < 0)
		{
			updated_l = true;
			ent_l->_move.y = - entity_l->_pos.y + 1e-3 ;
		}
		else if(entity_l->_pos.y + ent_l->_move.y > state_p.getWorldSize())
		{
			updated_l = true;
			ent_l->_move.y = state_p.getWorldSize() - entity_l->_pos.y ;
		}
	}
	return updated_l;
}

} // namespace octopus

