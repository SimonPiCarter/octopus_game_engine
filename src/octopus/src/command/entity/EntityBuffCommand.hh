#ifndef __EntityBuffCommand__
#define __EntityBuffCommand__

#include "command/Command.hh"
#include "state/Handle.hh"
#include "state/entity/buff/TimedBuff.hh"

namespace octopus
{

/// @brief Basic class that ensure move command with waypoints or not
/// Entity will be moved along all waypoints and command will terminate
/// when last one is reached
class EntityBuffCommand : public Command
{
public:
	EntityBuffCommand() {}
	EntityBuffCommand(Handle const &commandHandle_p, Handle const &target_p, TimedBuff const &buff_p);

	/// @brief
	virtual bool applyCommand(StepShallow & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

	TimedBuff const &getBuff() const { return _buff; }
private:
	Handle _target {0};

	TimedBuff _buff;
};

} // namespace octopus


#endif
