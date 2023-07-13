#include "CommandEffectOverTime.hh"

#include "step/Step.hh"
#include "step/command/data/TimerDataUpdateStep.hh"

namespace octopus
{

bool CommandEffectOverTime::applyCommand(Step & step_p, State const &state_p, CommandData const * data_p, CommandContext &commandContext_p) const
{
	TimerData const *timerData_l = dynamic_cast<TimerData const *>(data_p);
	step_p.addSteppable(new TimerDataUpdateStep(getHandle(), timerData_l->_time, timerData_l->_time+1));

	// Apply on each tick rate but only apply on start if boolean is enabled
	if(timerData_l->_time % _tickRate == 0
	&& (_applyOnStart || timerData_l->_time > 0))
	{
		applyEffect(step_p, state_p, data_p, commandContext_p);
	}

	return _tickRate * _nbOfTicks == timerData_l->_time && _nbOfTicks != 0;
}

} // namespace octopus
