#ifndef __Command__
#define __Command__

#include "CommandData.hh"
#include "state/Handle.hh"

namespace octopus
{
class Step;
class State;

/// @brief symbolise a command given (by player; by ai)
/// those commands must be serializable otherwise replay from file will not be possible
class Command
{
public:
	Command(Handle const &handle_p) : _handleCommand(handle_p) {}
	virtual ~Command() {}

	/// @brief refister the command into the step
	/// This method is responsible for
	/// handling cost of command and spawning command in step
	/// and checking legality
	virtual void registerCommand(Step & step_p, State const &state_p);

	/// @brief compile command or info into the step
	/// @return true if command is over
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const * data_p) const = 0;

	/// @brief create data supporting the command actions
	virtual CommandData * newData() const = 0;

	void setQueued(bool queued_p) { _queued = queued_p; }
	bool isQueued() const { return _queued; }
	Handle const &getHandleCommand() const { return _handleCommand; }

protected:
	/// @brief the commandable handle for the executor of this command
	Handle const _handleCommand {0};

	/// @brief if set to true the command will be queued up
	bool _queued { false };
};

} // namespace octopus


#endif
