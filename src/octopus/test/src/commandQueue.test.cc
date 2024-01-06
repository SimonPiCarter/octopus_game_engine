#include <gtest/gtest.h>

#include <vector>
#include <list>
#include <variant>

#include "command/CommandVar.hh"

struct Command
{
	virtual void registerCommand() const = 0;
	virtual bool applyCommand() const = 0;
	virtual void cleanUp() const {}
	virtual bool checkPlayer() const {return true;}
};

struct NoOpCommand : public Command
{
	virtual void registerCommand() const
	{
		std::cout<<"register No Op"<<std::endl;
	}
	virtual bool applyCommand() const
	{
		std::cout<<"apply No Op"<<std::endl;
		return true;
	}
};

struct CommandSimple : public Command
{
	virtual void registerCommand() const
	{
		std::cout<<"register Simple"<<std::endl;
	}
	virtual bool applyCommand() const
	{
		std::cout<<"apply Simple"<<std::endl;
		return true;
	}
};

using CommandVar = std::variant<NoOpCommand, CommandSimple>;

struct CommandBundle
{
	CommandVar _var;
	uint32_t _idx {0};
};

class CommandQueue
{
public:
	~CommandQueue();

	/// @brief returns true if at least one command has been queued
	/// @warning getCurrentCommand will throw if this return false
	bool hasCommand() const
	{
		return !_commandQueue.empty();
	}

	/// @brief return true if getCurrentCommand is different from getEnd
	bool hasCurrentCommand() const
	{
		return hasCommand();
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
	CommandBundle const &getBundle(uint32_t id_p) const
	{
		for(CommandBundle const & bundle_l: _commandQueue)
		{
			if(bundle_l._idx == id_p)
			{
				return bundle_l;
			}
		}
		throw std::logic_error("CommandQueue could not get bundle with id "+std::to_string(id_p));
	}
	/// @brief Get the bundle associated to the id
	CommandBundle &getBundle(uint32_t id_p)
	{
		for(CommandBundle & bundle_l: _commandQueue)
		{
			if(bundle_l._idx == id_p)
			{
				return bundle_l;
			}
		}
		throw std::logic_error("CommandQueue could not get bundle with id "+std::to_string(id_p));
	}

	std::list<CommandBundle> &getList()
	{
		return _commandQueue;
	}

	const std::list<CommandBundle> &getList() const
	{
		return _commandQueue;
	}

protected:
	/// @brief list of all actions in the command queue
	std::list<CommandBundle> _commandQueue;

	uint32_t _idx = 0;

	uint32_t getNextId()
	{
		return _idx++;
	}
};

void cleanUp(CommandVar const &var_p)
{
    std::visit([&](auto && arg) { arg.cleanUp(); }, var_p);
}

bool applyCommand(CommandVar const &var_p)
{
	bool result_l = false;
    std::visit([&](auto && arg) { result_l = arg.applyCommand(); }, var_p);
	return result_l;
}

CommandBundle const * iterateQueue(CommandBundle const *last_p, CommandQueue const &queue_p)
{
	if(!queue_p.hasCommand())
	{
		return nullptr;
	}
	std::list<CommandBundle>::const_iterator it_l = queue_p.getList().begin();
	CommandBundle const &bundle_l = *it_l;
	if(last_p->_idx != bundle_l._idx)
	{
		cleanUp(it_l->_var);
	}
	// while we have commands and the front one is over go on
	while(it_l != queue_p.getList().cend()
	   && applyCommand(it_l->_var))
	{
		// clean up
		cleanUp(it_l->_var);
		++it_l;
	}
	if(it_l == queue_p.getList().cend())
	{
		return nullptr;
	}
	return &*it_l;
}

TEST(commandQueue, simple)
{
	octopus::CommandVar var_l;
	EXPECT_TRUE(true);
}
