#include "CommandQueue.hh"
#include "command/Command.hh"

#include <stdexcept>

namespace octopus
{

bool CommandQueue::hasCommand() const
{
	return !_contextList.empty();
}

CommandBundle &CommandQueue::getFrontCommand()
{
	return *_contextList.front();
}

/// @brief return an iterator of the current command
/// this is the iterator stored in the front of _contextList
CommandQueue::ConstQueueIterator CommandQueue::getCurrentCommand() const
{
	if(_contextList.empty())
	{
		throw std::logic_error("Cannot access current command when no command have been added");
	}
	return _contextList.front();
}
/// @brief return the end iterator of the CommandQueue
CommandQueue::ConstQueueIterator CommandQueue::getEnd() const
{
	return _commandQueue.cend();
}

/// @brief update current context iterator to the next command
void CommandQueue::nextCommand()
{
	_contextList.front() = std::next(_contextList.front());
}
/// @brief update current context iterator to the previous command
void CommandQueue::prevCommand()
{
	_contextList.front() = std::prev(_contextList.front());
}

/// @brief this will queue the command and push a new context
/// the next command to be executed will be this command
void CommandQueue::queueCommand(Command *cmd_p)
{
	queueCommandLast(cmd_p);
	_contextList.push_front(std::prev(_commandQueue.end()));
}
/// @brief this will remove the last command of the queue
/// and restore the old context
void CommandQueue::unqueueCommand(Command *cmd_p)
{
	unqueueCommandLast(cmd_p);
	_contextList.pop_front();
}

/// @brief this will queue the command at the end of the current context
void CommandQueue::queueCommandLast(Command *cmd_p)
{
	_commandQueue.push_back({cmd_p, cmd_p->newData()});
	// if context queue was empty
	if(_commandQueue.size() == 1)
	{
		_contextList.push_front(std::prev(_commandQueue.end()));
	}
}
/// @brief this will just remove the last command of the queue
void CommandQueue::unqueueCommandLast(Command *cmd_p)
{
	// delete data but not command as it is shared between states and queues
	CommandBundle const &bundle_l = _commandQueue.back();
	if(bundle_l._cmd != cmd_p)
	{
		throw std::logic_error("CommandQueue was asked to remove a command that was not he last one");
	}
	delete bundle_l._data;
	_commandQueue.pop_back();
	// if context queue was empty
	if(_commandQueue.size() == 0)
	{
		_contextList.pop_front();
	}
}

} /// octopus
