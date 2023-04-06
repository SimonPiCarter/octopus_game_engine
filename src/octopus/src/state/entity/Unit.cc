#include "Unit.hh"

#include "logger/Logger.hh"

#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityBuffCommand.hh"
#include "state/State.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/EntityUpdateWaitingStep.hh"
#include "step/Step.hh"

namespace octopus
{

Unit::Unit(Vector const &pos_p, bool frozen_p, UnitModel const &model_p)
	: Entity(pos_p, frozen_p, model_p)
	, _unitModel(model_p)
{}

void Unit::runCommands(Step & step_p, State const &state_p, PathManager &pathManager_p)
{
	if(_waiting < 100000)
	{
		step_p.addSteppable(new EntityUpdateWaitingStep(_handle, _waiting, _waiting+1));
	}
	// if no command and buff we check for target
	if(!getQueue().hasCurrentCommand() && _unitModel._buffer._active )
	{
		Logger::getDebug() << " Entity::runCommands :: no command (buff)"<< std::endl;
		if(_waiting+1 >= _unitModel._buffer._reload)
		{
			// reset waiting time
			step_p.addSteppable(new EntityUpdateWaitingStep(_handle, _waiting, 0));
			// check for target
			Entity const * target_l = lookUpNewBuffTarget(state_p, _handle, _unitModel._buffer._range, _unitModel._buffer._buff);
			if(target_l)
			{
				// buff
				Logger::getDebug() << " Entity::runCommands :: add buff command" << _handle << " -> " << target_l->_handle <<"("<<_unitModel._buffer._buff._id<<")"<< std::endl;
				step_p.addSteppable(new CommandSpawnStep(new EntityBuffCommand(_commandableHandle, target_l->_handle, _unitModel._buffer._buff)));
			}
		}
	}
	// If no command we check for target if we have damage
	else if(!getQueue().hasCurrentCommand() && _model._damage > 1e-3)
	{
		Logger::getDebug() << " Entity::runCommands :: no command (attack)"<< std::endl;
		if(_waiting >= 50)
		{
			// reset waiting time
			step_p.addSteppable(new EntityUpdateWaitingStep(_handle, _waiting, 0));
			Entity const * target_l = lookUpNewTarget(state_p, _handle, 8);
			if(target_l)
			{
				Logger::getNormal() << " Entity::runCommands :: add attack command" << _handle << " -> " << target_l->_handle << std::endl;
				step_p.addSteppable(new CommandSpawnStep(new EntityAttackCommand(_commandableHandle, _handle, target_l->_handle, false)));
			}
		}
	}
	Commandable::runCommands(step_p, state_p, pathManager_p);
}

} // namespace octopus
