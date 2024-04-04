#include "CommandSpawnUnit.hh"

#include "step/Step.hh"
#include "step/command/data/TimerDataUpdateStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"

namespace octopus
{

bool CommandSpawnUnit::applyCommand(Step & step_p, State const &state_p, CommandData const * data_p, PathManager &pathManager_p) const
{
    Handle handle_l = getNextHandle(step_p, state_p);
    step_p.addSteppable(new UnitSpawnStep(handle_l, _unit));
	return true;
}

} // namespace octopus
