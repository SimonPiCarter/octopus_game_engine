#include "Commandable.hh"
#include "Command.hh"

namespace octopus
{
Commandable::~Commandable()
{
	for(CommandBundle &bundle_l : _queue)
	{
		delete bundle_l._data;
	}
}

void Commandable::enqueue(Command * cmd_p, bool enqueue_p)
{
	if(!enqueue_p)
	{
		_queue.clear();
	}
	_queue.push_back({cmd_p, cmd_p->newData()});
}

void Commandable::runCommands(Step & step_p, State const &state_p)
{
	// while we have commands and the front one is over go on
	while(!_queue.empty() && _queue.front()._cmd->applyCommand(step_p, state_p, _queue.front()._data))
	{
		// remove command
		_queue.pop_front();
	}
}

CommandBundle & Commandable::getFrontQueue()
{
	return _queue.front();
}

std::list<CommandBundle> &Commandable::getQueue()
{
	return _queue;
}

std::list<CommandBundle> const &Commandable::getQueue() const
{
	return _queue;
}


} // namespace octopus

