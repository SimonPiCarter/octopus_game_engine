#include "Command.hh"
#include "Commandable.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"

namespace octopus
{
void Command::registerCommand(Step &step_p, State const &)
{
	step_p.addSteppable(new CommandSpawnStep(this));
}

void Command::cleanUp(Step & , State const &, CommandData const *) const {}

void FlyingCommand::registerCommand(Step & , State const &)
{
	// NA for now
	throw std::logic_error("Should never be called");
}

}
