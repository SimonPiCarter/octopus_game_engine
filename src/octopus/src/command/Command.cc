#include "Command.hh"
#include "Commandable.hh"
#include "state/State.hh"
#include "state/player/Player.hh"
#include "state/entity/Entity.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"

namespace octopus
{
void Command::registerCommand(Step &step_p, State const &state_p)
{
	step_p.addSteppable(state_p, new CommandSpawnStep(this));
}

void Command::cleanUp(StepShallow & , State const &, CommandData const *) const {}

bool Command::checkPlayer(State const &state_p, unsigned long player_p) const
{
	return state_p.getEntity(this->getHandleCommand())
		&& state_p.getEntity(this->getHandleCommand())->_player == player_p;
}

void FlyingCommand::registerCommand(Step & , State const &)
{
	// NA for now
	throw std::logic_error("Should never be called");
}

}
