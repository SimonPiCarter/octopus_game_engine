
#include "EntityAttackCommand.hh"

#include <cmath>

#include "logger/Logger.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/entity/EntityAttackStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/entity/EntityMoveStep.hh"

namespace octopus
{

EntityAttackCommand::EntityAttackCommand(Handle const &commandHandle_p, Handle const &source_p, Handle const &target_p)
	: Command(commandHandle_p)
	, _source(source_p)
	, _target(target_p)
{}

bool EntityAttackCommand::applyCommand(Step & step_p, State const &state_p)
{
	// target disappeared or is dead
	bool targetMissing_l = checkTarget(state_p);
	if(targetMissing_l)
	{
		// reset wind up
		_windup = 0;
		// If target is dead we look for another target in range
		bool newTarget_l = lookUpNewTarget(state_p);

		// If no target we release
		if(!newTarget_l)
		{
			return true;
		}
	}

	Entity const * entSource_l = state_p.getEntity(_source);
	Entity const * entTarget_l = state_p.getEntity(_target);

	// If not in range we move to the target
	// if windup started we skip this
	if(!inRange(state_p) || _windup > 0)
	{
		// direction (source -> target)
		Vector dir_l = entTarget_l->_pos - entSource_l->_pos;
		// square distances
		double sqDistance_l = square_length(dir_l);
		// range squared (+ ray of target), we do not add ray of source because we might stop earlier to attack anyway plus move command
		// stops as soon as source touch target point
		double sqRange_l = (entSource_l->_stats._range + entTarget_l->_ray) * (entSource_l->_stats._range +  + entTarget_l->_ray);
		// ratio of direction to keep (square rooted)
		double ratio_l = std::sqrt(sqRange_l/sqDistance_l);

		Vector closest_l = entSource_l->_pos + dir_l * (1 - ratio_l);

		// add move command
		step_p.addEntityMoveStep(new EntityMoveStep(createEntityMoveStep(*entSource_l, closest_l, entSource_l->_stepSpeed)));

		// reset wind up (time before attack)
		_windup = 0;
	}
	else if(entSource_l->_stats._reload >= entSource_l->_stats._fullReload )
	{
		// If in range we trigger the attack (delay may be applied for animation)
		if(_windup >= entSource_l->_stats._windup)
		{
			// reset wind up
			_windup = 0;

			// add damage
			step_p.addSteppable(new EntityHitPointChangeStep(_target, entSource_l->_stats._damage - entTarget_l->_stats._armor));
			// reset reload time
			step_p.addSteppable(new EntityAttackStep(_source, entSource_l->_stats._reload));
		}
		_windup += 1;
	}

	return false;
}

bool EntityAttackCommand::checkTarget(State const &state_p) const
{
	Entity const * entTarget_l = state_p.getEntity(_target);
	return entTarget_l->_alive;
}

bool EntityAttackCommand::lookUpNewTarget(State const &state_p) const
{
	/// @todo implement this
	return false;
}

bool EntityAttackCommand::inRange(State const &state_p) const
{
	Entity const * entSource_l = state_p.getEntity(_source);
	Entity const * entTarget_l = state_p.getEntity(_target);

	// direction (source -> target)
	Vector dir_l = entTarget_l->_pos - entSource_l->_pos;
	// square distances
	double sqDistance_l = square_length(dir_l);
	// range squared (+ rays)
	double rangeAndRays_l = entSource_l->_stats._range + entTarget_l->_ray + entSource_l->_ray;
	double sqRange_l = rangeAndRays_l * rangeAndRays_l;

	return sqDistance_l < sqRange_l + 1e-5;
}

} // namespace octopus

