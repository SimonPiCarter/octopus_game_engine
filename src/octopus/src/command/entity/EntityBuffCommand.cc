
#include "EntityBuffCommand.hh"

#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Unit.hh"
#include "step/Step.hh"
#include "step/entity/buff/EntityBuffStep.hh"
#include "step/entity/EntityUpdateReloadAbilityStep.hh"

namespace octopus
{

EntityBuffCommand::EntityBuffCommand(Handle const &commandHandle_p, Handle const &target_p, TimedBuff const &buff_p)
	: Command(commandHandle_p)
	, _target(target_p)
	, _buff(buff_p)
{}

bool EntityBuffCommand::applyCommand(StepShallow & step_p, State const &state_p, CommandData const *, PathManager &) const
{
	Logger::getDebug() << "EntityBuffCommand:: apply Command "<<_target <<std::endl;

	Unit const * unit_l = dynamic_cast<Unit const *>(state_p.getEntity(_handleCommand));

	std::string const &buffId_l = _buff._id;
	unsigned long reload_l = getReloadAbilityTime(*unit_l, buffId_l, unit_l->_unitModel._buffer._reload);

	if(reload_l+1 >= unit_l->_unitModel._buffer._reload)
	{
		// Use & reset cooldown
		step_p.addSteppable(new EntityBuffStep(_target, _buff));

		step_p.addSteppable(new EntityUpdateReloadAbilityStep(_handleCommand, buffId_l, reload_l, 0));

		return true;
	}

	return false;
}

} // namespace octopus

