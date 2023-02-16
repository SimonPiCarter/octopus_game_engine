#ifndef __CommandQueue__
#define __CommandQueue__

#include <list>

namespace octopus
{

class Command;
class CommandData;

struct CommandBundle
{
	Command * _cmd {nullptr};
	CommandData * _data {nullptr};
};

/// @brief This class represent a command queue
/// enabling step for modification
/// Supported oprations are
/// - queue up new command after the current list
/// - queue up command in place of the current list
class CommandQueue
{
public:
	typedef std::list<CommandBundle>::const_iterator ConstQueueIterator;
	typedef std::list<CommandBundle>::iterator QueueIterator;

	~CommandQueue();

	/// @brief returns true if at least one command has been queued
	/// @warning getCurrentCommand will throw if this return false
	bool hasCommand() const;

	/// @brief return true if getCurrentCommand is different from getEnd
	bool hasCurrentCommand() const;

	CommandBundle &getFrontCommand();
	CommandBundle const &getFrontCommand() const;
	/// @brief return an iterator of the current command
	/// this is the iterator stored in the front of _contextList
	ConstQueueIterator getCurrentCommand() const;
	/// @brief return the end iterator of the CommandQueue
	ConstQueueIterator getEnd() const;

	/// @brief update current context iterator to the next command
	void nextCommand();
	/// @brief update current context iterator to the previous command
	void prevCommand();

	/// @brief this will queue the command and push a new context
	/// the next command to be executed will be this command
	void queueCommand(Command *cmd_p);
	/// @brief this will remove the last command of the queue
	/// and restore the old context
	void unqueueCommand(Command *cmd_p);

	/// @brief this will queue the command at the end of the current context
	void queueCommandLast(Command *cmd_p);
	/// @brief this will just remove the last command of the queue
	void unqueueCommandLast(Command *cmd_p);

protected:
	/// @brief list of all actions in the command queue
	std::list<CommandBundle> _commandQueue;

	/// @brief list of the successive iterator in the command queue
	/// This allow poping front element to immediatly retrieve last known position in the command queue
	std::list<QueueIterator> _contextList;
};

} // namespace octopus


#endif
