#ifndef __EntityBuffCommand__
#define __EntityBuffCommand__

#include "Command.hh"
#include "state/Handle.hh"
#include "state/entity/Buff.hh"

namespace octopus
{

/// @brief Basic class that ensure move command with waypoints or not
/// Entity will be moved along all waypoints and command will terminate
/// when last one is reached
class EntityBuffCommand : public Command
{
public:
	EntityBuffCommand(Handle const &commandHandle_p, Handle const &target_p, TyppedBuff const &buff_p);

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const override;

	virtual CommandData * newData() const override { return nullptr; }
private:
	Handle const _target;

	TyppedBuff const _buff;
};

} // namespace octopus


#endif
