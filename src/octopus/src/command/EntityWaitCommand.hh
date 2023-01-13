#ifndef __EntityWaitCommand__
#define __EntityWaitCommand__

#include "Command.hh"
#include "state/Handle.hh"


namespace octopus
{

/// @brief Basic class that ensure move command with waypoints or not
/// Entity will be moved along all waypoints and command will terminate
/// when last one is reached
class EntityWaitCommand : public Command
{
public:
	EntityWaitCommand(Handle const &handle_p);

	/// @brief
	virtual bool registerCommand(Step & step_p, State const &state_p) override;
private:
	Handle const _handle;
};

} // namespace octopus


#endif
