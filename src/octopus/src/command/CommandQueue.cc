#include "CommandQueue.hh"
#include "command/Command.hh"

#include <stdexcept>
#include <string>

namespace octopus
{

CommandQueue::~CommandQueue()
{
	for(CommandBundle const &bundle_l : _commandQueue)
	{
		delete bundle_l._data;
	}
}

bool CommandQueue::hasCommand() const
{
	return !_contextList.empty();
}

bool CommandQueue::hasCurrentCommand() const
{
	return hasCommand() && getCurrentCommand() != getEnd();
}

CommandBundle &CommandQueue::getFrontCommand()
{
	return *_contextList.front();
}

CommandBundle const &CommandQueue::getFrontCommand() const
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
	bool isEmpty_l = _commandQueue.empty() || getCurrentCommand() == getEnd();
	_commandQueue.push_back({cmd_p, cmd_p->newData(), _commandQueue.size()});
	// if context queue was empty
	if(isEmpty_l)
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
	if(_commandQueue.empty())
	{
		_contextList.pop_front();
	}
}

CommandBundle const &CommandQueue::getBundle(size_t id_p) const
{
	for(CommandBundle const & bundle_l: _commandQueue)
	{
		if(bundle_l._id == id_p)
		{
			return bundle_l;
		}
	}
	throw std::logic_error("CommandQueue could not get bundle with id "+std::to_string(id_p));
}

} /// octopus
