#ifndef __Command__
#define __Command__

#include "CommandData.hh"
#include "state/Handle.hh"

namespace octopus
{
class Step;
class State;

class Command
{
public:
	Command(Handle const &handle_p) : _handleCommand(handle_p) {}

	void registerCommand(Step & step_p);

	/// @brief register new commands or info into the step
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
