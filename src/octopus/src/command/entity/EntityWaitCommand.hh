#ifndef __EntityWaitCommand__
#define __EntityWaitCommand__

#include "command/Command.hh"
#include "state/Handle.hh"


namespace octopus
{

/// @brief Basic class that ensure move command with waypoints or not
/// Entity will be moved along all waypoints and command will terminate
/// when last one is reached
class EntityWaitCommand : public Command
{
public:
	EntityWaitCommand() {}
	EntityWaitCommand(Handle const &commandHandle_p, Handle const &source_p);

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

private:
	Handle _source {0};
};

} // namespace octopus


#endif
