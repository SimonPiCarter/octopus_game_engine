#include "Commandable.hh"
#include "command/Command.hh"

#include "step/command/CommandQueueStep.hh"
#include "step/Step.hh"

namespace octopus
{
Commandable::Commandable()
{}

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

void Commandable::runCommands(Step & step_p, State const &state_p, PathManager &pathManager_p)
{
	if(!_queue.hasCommand())
	{
		return;
	}
	CommandQueue::ConstQueueIterator it_l = _queue.getCurrentCommand();
	if(_hasLastCommand && it_l != _queue.getEnd() && _lastCommand != it_l->_id)
	{
		CommandBundle const & cmd_l = _queue.getBundle(_lastCommand);
		cmd_l._cmd->cleanUp(step_p, state_p, cmd_l._data);
	}
	// while we have commands and the front one is over go on
	while(it_l != _queue.getEnd()
	   && it_l->_cmd->applyCommand(step_p, state_p, it_l->_data, pathManager_p))
	{
		// clean up
		it_l->_cmd->cleanUp(step_p, state_p, it_l->_data);
		step_p.addSteppable(new CommandNextStep(_commandableHandle));
		++it_l;
	}
	_hasLastCommand = true;
	if(it_l != _queue.getEnd())
	{
		step_p.addSteppable(new CommandUpdateLastIdStep(_commandableHandle, _lastCommand, it_l->_id));
	}
}

CommandBundle & Commandable::getFrontQueue()
{
	return _queue.getFrontCommand();
}

CommandBundle const & Commandable::getFrontQueue() const
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

void Commandable::setIdLast(size_t id_p)
{
	_lastCommand = id_p;
}


} // namespace octopus

