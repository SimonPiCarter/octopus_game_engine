
#include "EntityAttackCommand.hh"

#include <cmath>

#include "command/data/AttackMoveData.hh"
#include "logger/Logger.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/command/CommandWindUpDiffStep.hh"
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

bool EntityAttackCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const
{
	// get data
	AttackMoveData const &data_l = *static_cast<AttackMoveData const *>(data_p);
	long windup_l = data_l._windup;
	Handle const &curTarget_l = data_l._target;

	Logger::getDebug() << "EntityAttackCommand:: apply Command "<<_source << " -> " <<curTarget_l<<std::endl;

	// target disappeared or is dead
	bool targetMissing_l = checkTarget(state_p, curTarget_l);
	if(targetMissing_l)
	{
		// reset wind up
		step_p.addSteppable(new CommandWindUpDiffStep(_handleCommand, - windup_l));
		// If target is dead we look for another target in range
		Entity const * newTarget_l = lookUpNewTarget(state_p, _source);

		// If no target we release
		if(!newTarget_l)
		{
			return true;
		}
		else
		{
			// steppable to update target
			// TODO
			return false;
		}
	}

	Entity const * entSource_l = state_p.getEntity(_source);
	Entity const * entTarget_l = state_p.getEntity(curTarget_l);

	// If not in range we move to the target
	// if windup started we skip this
	if(!inRange(state_p, curTarget_l) && windup_l == 0)
	{
		Logger::getDebug() << "\tEntityAttackCommand:: not in range"<<std::endl;
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

		Logger::getDebug() << "\t\tEntityAttackCommand:: adding move step "<< _source << " target " << closest_l << " speed " <<entSource_l->_stepSpeed<<std::endl;
		// add move command
		step_p.addEntityMoveStep(new EntityMoveStep(createEntityMoveStep(*entSource_l, closest_l, entSource_l->_stepSpeed)));
	}
	else if(entSource_l->_stats._reload >= entSource_l->_stats._fullReload )
	{
		step_p.addSteppable(new CommandWindUpDiffStep(_handleCommand, 1));
		Logger::getDebug() << "\tEntityAttackCommand:: in range (winding up)"<<std::endl;
		// If in range we trigger the attack (delay may be applied for animation)
		// + 1 to take into account steppable added just before
		if(windup_l +1 >= entSource_l->_stats._windup)
		{
			Logger::getDebug() << "\tEntityAttackCommand:: in range (attack)"<<std::endl;
			// reset wind up (remove value + 1 because step +1 will be applied before resetting)
			step_p.addSteppable(new CommandWindUpDiffStep(_handleCommand, - windup_l - 1));

			// add damage
			step_p.addSteppable(new EntityHitPointChangeStep(_target, std::min(-1., entTarget_l->_stats._armor - entSource_l->_stats._damage)));
			// reset reload time
			step_p.addSteppable(new EntityAttackStep(_source, entSource_l->_stats._reload));
		}
	}

	return false;
}

bool EntityAttackCommand::checkTarget(State const &state_p, Handle const & target_p) const
{
	Entity const * entTarget_l = state_p.getEntity(target_p);
	return !entTarget_l->_alive;
}

bool EntityAttackCommand::inRange(State const &state_p, Handle const & target_p) const
{
	Entity const * entSource_l = state_p.getEntity(_source);
	Entity const * entTarget_l = state_p.getEntity(target_p);

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

