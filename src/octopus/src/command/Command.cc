#include "Command.hh"
#include "Commandable.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"

namespace octopus
{
void Command::registerCommand(Step &step_p)
{
	step_p.addSteppable(new CommandSpawnStep(_handleCommand, this, _queued));
}
}
