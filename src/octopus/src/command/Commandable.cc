#include "Commandable.hh"
#include "command/Command.hh"

#include "step/command/CommandQueueStep.hh"
#include "step/Step.hh"

namespace octopus
{
Commandable::~Commandable()
{}

void Commandable::enqueue(Command * cmd_p, bool enqueue_p)
{
	if(!enqueue_p)
	{
		_queue.queueCommand(cmd_p);
	}
	else
	{
		_queue.queueCommandLast(cmd_p);
	}
}

bool Commandable::isActive() const
{
	return true;
}

void Commandable::runCommands(Step & step_p, State const &state_p)
{
	if(!_queue.hasCommand())
	{
		return;
	}
	CommandQueue::ConstQueueIterator it_l = _queue.getCurrentCommand();
	// while we have commands and the front one is over go on
	while(it_l != _queue.getEnd()
	   && it_l->_cmd->applyCommand(step_p, state_p, it_l->_data))
	{
		++it_l;
		step_p.addSteppable(new CommandNextStep(_commandableHandle));
	}
}

CommandBundle & Commandable::getFrontQueue()
{
	return _queue.getFrontCommand();
}

CommandQueue &Commandable::getQueue()
{
	return _queue;
}

const CommandQueue &Commandable::getQueue() const
{
	return _queue;
}


} // namespace octopus

