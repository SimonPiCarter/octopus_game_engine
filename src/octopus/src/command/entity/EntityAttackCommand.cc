
#include "EntityAttackCommand.hh"

#include <cmath>

#include "command/data/AttackMoveData.hh"
#include "logger/Logger.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/command/CommandWindUpDiffStep.hh"
#include "step/command/CommandNewTargetStep.hh"
#include "step/command/CommandIncrementNoProgressStep.hh"
#include "step/command/CommandUpdateLastPosStep.hh"
#include "step/entity/EntityAttackStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/entity/EntityMoveStep.hh"
#include "step/entity/EntityFrozenStep.hh"


namespace octopus
{

EntityAttackCommand::EntityAttackCommand(Handle const &commandHandle_p, Handle const &source_p, Handle const &target_p, bool frozenTarget_p)
	: Command(commandHandle_p)
	, _source(source_p)
	, _target(target_p)
	, _frozenTarget(frozenTarget_p)
{}

bool EntityAttackCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const
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
			Logger::getDebug() << "EntityAttackCommand:: no new target found "<<std::endl;
			return true;
		}
		else
		{
			Logger::getDebug() << "EntityAttackCommand:: new target found "<<newTarget_l->_handle<<std::endl;
			/// steppable to update target
			step_p.addSteppable(new CommandNewTargetStep(_handleCommand, newTarget_l->_handle, curTarget_l));
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
		Fixed sqDistance_l = square_length(dir_l);
		// range squared (+ ray of target), we do not add ray of source because we might stop earlier to attack anyway plus move command
		// stops as soon as source touch target point
		Fixed sqRange_l = (entSource_l->_model._range + entTarget_l->_model._ray) * (entSource_l->_model._range +  + entTarget_l->_model._ray);
		// ratio of direction to keep (square rooted)
		Fixed ratio_l = numeric::square_root(sqRange_l/sqDistance_l);

		Vector closest_l = entSource_l->_pos + dir_l * (1. - ratio_l);

		/// @todo check for closer target to reallocate aggro

		// if entity is frozen and not in range it means target has run out of range
		// scan for target closer only if target is not frozen
		if(entSource_l->_frozen && !_frozenTarget)
		{
			// If target is dead we look for another target in range
			Entity const * newTarget_l = lookUpNewTarget(state_p, _source);

			// If target we update
			if(newTarget_l && newTarget_l->_handle != curTarget_l)
			{
				Logger::getDebug() << "EntityAttackCommand:: new target found (out of range) "<<newTarget_l->_handle<<std::endl;
				/// steppable to update target
				step_p.addSteppable(new CommandNewTargetStep(_handleCommand, newTarget_l->_handle, curTarget_l));
			}
		}

		/// @todo if no closer target try to move randomly (orthogonal move)

		Logger::getDebug() << "\t\tEntityAttackCommand:: adding move step "<< _source << " target " << closest_l << " speed " <<entSource_l->getStepSpeed()<<std::endl;
		// add move command
		step_p.addEntityMoveStep(new EntityMoveStep(createEntityMoveStep(*entSource_l, closest_l, entSource_l->getStepSpeed())));
	}
	else if(entSource_l->_reload >= entSource_l->getFullReload() )
	{
		step_p.addSteppable(new CommandWindUpDiffStep(_handleCommand, 1));
		Logger::getDebug() << "\tEntityAttackCommand:: in range (winding up)"<<std::endl;
		// If in range we trigger the attack (delay may be applied for animation)
		// + 1 to take into account steppable added just before
		if(windup_l +1 >= entSource_l->_model._windup)
		{
			Logger::getDebug() << "\tEntityAttackCommand:: in range (attack)"<<std::endl;
			// reset wind up (remove value + 1 because step +1 will be applied before resetting)
			step_p.addSteppable(new CommandWindUpDiffStep(_handleCommand, - windup_l - 1));

			// add damage
			step_p.addSteppable(new EntityHitPointChangeStep(curTarget_l, std::min(-1., entTarget_l->getArmor() - entSource_l->getDamage(entTarget_l->_model))));
			// reset reload time
			step_p.addSteppable(new EntityAttackStep(_source, entSource_l->_reload));

			// once we trigger attack if we attack a building check for a new target
			if(entTarget_l->_model._isBuilding)
			{
				// If target is dead we look for another target in range
				Entity const * newTarget_l = lookUpNewTarget(state_p, _source);
				if(newTarget_l && newTarget_l->_model._isUnit)
				{
					Logger::getDebug() << "EntityAttackCommand:: new target found (out of range) "<<newTarget_l->_handle<<std::endl;
					/// steppable to update target
					step_p.addSteppable(new CommandNewTargetStep(_handleCommand, newTarget_l->_handle, curTarget_l));
				}
			}
		}
	}
	else
	{
		Logger::getDebug() << "\tEntityAttackCommand:: in range (reloading)"<<std::endl;
	}

	if(inRange(state_p, curTarget_l) || windup_l > 0)
	{
		step_p.addSteppable(new EntityFrozenStep(_source, entSource_l->_frozen, true));
	}
	else
	{
		step_p.addSteppable(new EntityFrozenStep(_source, entSource_l->_frozen, false));
	}

	return false;
}

void EntityAttackCommand::cleanUp(Step & step_p, State const &state_p, CommandData const *) const
{
	Entity const * entSource_l = state_p.getEntity(_source);
	step_p.addSteppable(new EntityFrozenStep(_source, entSource_l->_frozen, false));
}

Handle const &EntityAttackCommand::getSource() const
{
	return _source;
}
Handle const &EntityAttackCommand::getTarget() const
{
	return _target;
}
bool EntityAttackCommand::isFrozenTarget() const
{
	return _frozenTarget;
}

bool EntityAttackCommand::checkTarget(State const &state_p, Handle const & target_p) const
{
	Entity const * entTarget_l = state_p.getEntity(target_p);
	return !entTarget_l->_alive || entTarget_l->_model._invulnerable;
}

bool EntityAttackCommand::inRange(State const &state_p, Handle const & target_p) const
{
	Entity const * entSource_l = state_p.getEntity(_source);
	Entity const * entTarget_l = state_p.getEntity(target_p);

	// direction (source -> target)
	Vector dir_l = entTarget_l->_pos - entSource_l->_pos;
	// square distances
	Fixed sqDistance_l = square_length(dir_l);
	// range squared (+ rays)
	Fixed rangeAndRays_l = entSource_l->_model._range + entTarget_l->_model._ray + entSource_l->_model._ray;
	Fixed sqRange_l = rangeAndRays_l * rangeAndRays_l;

	return sqDistance_l < sqRange_l + 1e-5;
}

} // namespace octopus

