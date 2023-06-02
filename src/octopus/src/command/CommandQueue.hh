#ifndef __CommandQueue__
#define __CommandQueue__

#include <cstddef>
#include <list>
#include "CommandVar.hh"

namespace octopus
{

class Command;
class CommandData;

/// @brief This class represent a command queue
/// enabling step for modification
/// Supported oprations are
/// - queue up new command after the current list
/// - queue up command in place of the current list

struct CommandBundle
{
	CommandVar _var;
	size_t _idx {0};
};

class CommandQueue
{
public:
	~CommandQueue() {}

	/// @brief returns true if at least one command has been queued
	bool hasCommand() const
	{
		return !_commandQueue.empty();
	}

	CommandBundle &getFrontCommand()
	{
		return _commandQueue.front();
	}
	CommandBundle const &getFrontCommand() const
	{
		return _commandQueue.front();
	}

	/// @brief update current context iterator to the next command
	void nextCommand()
	{
		_commandQueue.pop_front();
	}
	/// @brief update current context iterator to the previous command
	void prevCommand(CommandBundle const &cmd_p)
	{
		_commandQueue.push_front(cmd_p);
	}

	/// @brief this will queue the command and push a new context
	/// the next command to be executed will be this command
	void queueCommand(CommandVar const &cmd_p)
	{
		_commandQueue.clear();
		queueCommandLast(cmd_p);
	}
	/// @brief this will remove the last command of the queue
	/// and restore the old context
	void unqueueCommand(std::list<CommandBundle> const &old_p)
	{
		_commandQueue = old_p;
	}

	/// @brief this will queue the command at the end of the current context
	void queueCommandLast(CommandVar const &cmd_p)
	{
		_commandQueue.push_back({cmd_p, getNextId()});
	}
	/// @brief this will just remove the last command of the queue
	void unqueueCommandLast()
	{
		_commandQueue.pop_back();
	}

	/// @brief Get the bundle associated to the id
	CommandBundle const *getBundle(size_t id_p) const
	{
		for(CommandBundle const & bundle_l: _commandQueue)
		{
			if(bundle_l._idx == id_p)
			{
				return &bundle_l;
			}
		}
		return nullptr;
	}
	/// @brief Get the bundle associated to the id
	CommandBundle *getBundle(size_t id_p)
	{
		for(CommandBundle & bundle_l: _commandQueue)
		{
			if(bundle_l._idx == id_p)
			{
				return &bundle_l;
			}
		}
		return nullptr;
	}

	std::list<CommandBundle> &getList()
	{
		return _commandQueue;
	}

	const std::list<CommandBundle> &getList() const
	{
		return _commandQueue;
	}

	/// @brief return the set of registration of the current command in queue
	/// @note return 0 if no command in queue
	unsigned long long getStepOfRegistration() const;

protected:
	/// @brief list of all actions in the command queue
	std::list<CommandBundle> _commandQueue;

	/// @brief used to set up command bundles unique identifiers
	size_t _idx = 0;

	size_t getNextId()
	{
		return _idx++;
	}
};

} // namespace octopus


#endif
