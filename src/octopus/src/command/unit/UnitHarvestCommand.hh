#ifndef __UnitHarvestCommand__
#define __UnitHarvestCommand__

#include "command/Command.hh"
#include "state/Handle.hh"
#include "command/data/HarvestMoveData.hh"
#include "command/entity/EntityMoveCommand.hh"

namespace octopus
{

/// @brief Command that make the unit harvest and drop automatically
/// will also look for new resources close to exhausted one automatically
class UnitHarvestCommand : public Command
{
public:
	UnitHarvestCommand(Handle const &commandHandle_p, Handle const &source_p, Handle const &resource_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p, bool init_p=false);

	virtual void registerCommand(Step & step_p, State const &state_p);

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

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
