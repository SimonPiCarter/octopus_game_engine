#ifndef __Commandable__
#define __Commandable__

#include "state/Handle.hh"
#include "CommandQueue.hh"
#include <list>

namespace octopus
{
class Step;
class State;
class Command;


/// @brief This class handle commandles :
/// everything that can take commands
/// On a given state and step all commands on front of the queue
/// will register their steps to build the step content
/// once a command is over remove it from the queue
class Commandable
{
public:
	virtual ~Commandable();

	/// @brief add the command to the queue
	/// @param cmd_p
	/// @param enqueue_p if true the cmd is added on top of the queue else it overrides it
	void enqueue(Command * cmd_p, bool enqueue_p);

	/// @brief is the commandable active in the state
	/// @note should only return true if the commandable should be run
	virtual bool isActive() const;

	/// @brief run the commands in queue and pop them if terminated
	virtual void runCommands(Step & step_p, State const &state_p);

	CommandBundle & getFrontQueue();
	CommandQueue & getQueue();
	const CommandQueue & getQueue() const;

	Handle _commandableHandle {0};
private:
	CommandQueue _queue;
};

} // namespace octopus


#endif
