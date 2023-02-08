#include "Unit.hh"

#include "logger/Logger.hh"

#include "command/entity/EntityAttackCommand.hh"
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

void Unit::runCommands(Step & step_p, State const &state_p)
{
	// If no command we check for target if we have damage
	if(!getQueue().hasCurrentCommand() && _model._damage > 1e-3)
	{
		Logger::getDebug() << " Entity::runCommands :: no command"<< std::endl;
		if(_waiting >= 50)
		{
			// reset waiting time
			step_p.addSteppable(new EntityUpdateWaitingStep(_handle, _waiting, 0));
			Entity const * target_l = lookUpNewTarget(state_p, _handle);
			if(target_l)
			{
				Logger::getDebug() << " Entity::runCommands :: add attack command" << _handle << " -> " << target_l->_handle << std::endl;
				step_p.addSteppable(new CommandSpawnStep(new EntityAttackCommand(_commandableHandle, _handle, target_l->_handle)));
			}
		}
		else
		{
			step_p.addSteppable(new EntityUpdateWaitingStep(_handle, _waiting, _waiting+1));
		}
	}
	Commandable::runCommands(step_p, state_p);
}

} // namespace octopus
