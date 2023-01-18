#ifndef __UnitHarvestCommand__
#define __UnitHarvestCommand__

#include "Command.hh"
#include "state/Handle.hh"
#include "command/data/HarvestMoveData.hh"
#include "command/EntityMoveCommand.hh"

namespace octopus
{

/// @brief Basic class that ensure move command with waypoints or not
/// Entity will be moved along all waypoints and command will terminate
/// when last one is reached
class UnitHarvestCommand : public Command
{
public:
	UnitHarvestCommand(Handle const &commandHandle_p, Handle const &source_p, Handle const &resource_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p);

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const override;

	virtual CommandData * newData() const override
	{
		return new HarvestMoveData(_resource, 0, _subMoveCommand.getFinalPoint(), _subMoveCommand.getGridStatus(), _subMoveCommand.getWaypoints());
	}
private:
	Handle const _source;
	Handle const _resource;

	/// @brief sub move command
	EntityMoveCommand _subMoveCommand;
};

} // namespace octopus


#endif
