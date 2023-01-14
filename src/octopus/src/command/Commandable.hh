#ifndef __Commandable__
#define __Commandable__

#include "state/Handle.hh"
#include <list>

namespace octopus
{
class Step;
class State;
class Command;
class CommandData;

struct CommandBundle
{
	Command * _cmd;
	CommandData * _data;
};

/// @brief This class handle commandles :
/// everything that can take commands
/// On a given state and step all commands on front of the queue
/// will register their steps to build the step content
/// once a command is over remove it from the queue
class Commandable
{
public:
	~Commandable();

	/// @brief add the command to the queue
	/// @param cmd_p
	/// @param enqueue_p if true the cmd is added on top of the queue else it overrides it
	void enqueue(Command * cmd_p, bool enqueue_p);

	/// @brief run the commands in queue and pop them if terminated
	void runCommands(Step & step_p, State const &state_p);

	CommandBundle & getFrontQueue();
	std::list<CommandBundle> &getQueue();
	std::list<CommandBundle> const &getQueue() const;

	Handle _commandableHandle {0};
private:
	std::list<CommandBundle> _queue;
};

} // namespace octopus


#endif
