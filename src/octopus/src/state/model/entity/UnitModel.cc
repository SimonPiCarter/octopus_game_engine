#include "UnitModel.hh"

#include "logger/Logger.hh"

#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityBuffCommand.hh"
#include "state/State.hh"
#include "state/entity/Unit.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/entity/EntityUpdateWaitingStep.hh"
#include "step/Step.hh"

namespace octopus
{

UnitModel::UnitModel(bool isStatic_p, Fixed ray_p, Fixed stepSpeed_p, Fixed hpMax_p)
	: EntityModel(isStatic_p, ray_p, stepSpeed_p, hpMax_p)
{
	_idleFunc = unitIdleFunction;
	_isUnit = true;
}

void unitIdleFunction(Entity const &ent_p, Step & step_p, State const &state_p)
{
	Unit const &unit_l = static_cast<Unit const &>(ent_p);
	// if no command and buff we check for target
	if(unit_l._unitModel._buffer._active )
	{
		Logger::getDebug() << " Unit::runCommands :: no command (buff)"<< std::endl;
		if(unit_l._waiting+1 >= unit_l._unitModel._buffer._reload)
		{
			// reset waiting time
			step_p.addSteppable(new EntityUpdateWaitingStep(unit_l._handle, unit_l._waiting, 0));
			// check for target
			Entity const * target_l = lookUpNewBuffTarget(state_p, unit_l._handle, unit_l._unitModel._buffer._range, unit_l._unitModel._buffer._buff);
			if(target_l)
			{
				// buff
				Logger::getDebug() << " Unit::runCommands :: add buff command" << unit_l._handle << " -> " << target_l->_handle <<"("<<unit_l._unitModel._buffer._buff._id<<")"<< std::endl;
				step_p.addSteppable(new CommandSpawnStep(new EntityBuffCommand(unit_l._commandableHandle, target_l->_handle, unit_l._unitModel._buffer._buff)));
			}
		}
	}
	// If no command we check for target if we have damage
	else if(unit_l._model._damage > 1e-3)
	{
		Logger::getDebug() << " Unit::runCommands :: no command (attack)"<< std::endl;
		if(unit_l._waiting >= 50)
		{
			// reset waiting time
			step_p.addSteppable(new EntityUpdateWaitingStep(unit_l._handle, unit_l._waiting, 0));
			Entity const * target_l = lookUpNewTarget(state_p, unit_l._handle, unit_l._aggroDistance);
			if(target_l)
			{
				Logger::getDebug() << " Unit::runCommands :: add attack command" << unit_l._handle << " -> " << target_l->_handle << std::endl;
				step_p.addSteppable(new CommandSpawnStep(new EntityAttackCommand(unit_l._commandableHandle, unit_l._handle, target_l->_handle, false)));
			}
		}
	}
}

} // namespace octopus

