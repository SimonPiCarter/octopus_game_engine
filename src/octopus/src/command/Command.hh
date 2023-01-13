#ifndef __Command__
#define __Command__

#include <array>
#include "state/Handle.hh"
#include "state/State.hh"

namespace octopus
{
class Step;
class State;

class Command
{
public:
	Command(Handle const &handle_p) : _handleCommand(handle_p) {}

	void registerCommand(State &state_p);

	/// @brief register new commands or info into the step
	/// @return true if command is over
	virtual bool applyCommand(Step & step_p, State const &state_p) = 0;

private:
	/// @brief the commandable handle for the executor of this command
	Handle const _handleCommand {0};

	/// @brief if set to true the command will be queued up
	bool _queued { false };
};


template<typename data>
class CommandWithData : public Command
{
public:
	CommandWithData(Handle const &handle_p, data const &data_p) : Command(handle_p), _metaData({data_p, data_p, data_p}) {}

protected:
	data & getMetaData(State const &state_p)
	{
		return _metaData.at(state_p._id);
	}

private:
	/// @brief meta data
	std::array<data, 3> _metaData;
};


} // namespace octopus


#endif
