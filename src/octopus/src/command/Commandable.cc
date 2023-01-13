#include "Commandable.hh"
#include "Command.hh"

namespace octopus
{
void Commandable::enqueue(Command * cmd_p, bool enqueue_p)
{
	if(!enqueue_p)
	{
		_queue.clear();
	}
	_queue.push_back(cmd_p);
}

void Commandable::runCommands(Step & step_p, State const &state_p)
{
	// while we have commands and the front one is over go on
	while(!_queue.empty() && _queue.front()->applyCommand(step_p, state_p))
	{
		// remove command
		_queue.pop_front();
	}
}

std::list<Command *> &Commandable::getQueue()
{
	return _queue;
}

std::list<Command *> const &Commandable::getQueue() const
{
	return _queue;
}


} // namespace octopus

