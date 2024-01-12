
#include "EntityRallyPointCommand.hh"

#include "step/Step.hh"
#include "command/CommandHelpers.hh"
#include "step/command/CommandQueueStep.hh"

namespace octopus
{

EntityRallyPointCommand::EntityRallyPointCommand(Handle const &source_p, Handle const &target_p, Vector const &pos_p, bool targetNotSet_p)
	: Command(source_p)
	, _target(target_p)
	, _pos(pos_p)
	, _targetNotSet(targetNotSet_p)
{}

bool EntityRallyPointCommand::applyCommand(StepShallow & step_p, State const &state_p, CommandData const *, PathManager &) const
{
	Command *cmd_l = newTargetCommand(state_p, _handleCommand, _target, _pos, _targetNotSet);
	if(cmd_l)
	{
		step_p.addSteppable(new CommandSpawnStep(cmd_l));
	}

	// false here is required because if a next step is issued on the same step as a new command is spawn
	// the newly command is discarded
	return false;
}

} // namespace octopus

