#ifndef __BuildingCommand__
#define __BuildingCommand__

#include "command/Command.hh"
#include "command/data/MoveData.hh"
#include "EntityMoveCommand.hh"
#include "state/Handle.hh"

namespace octopus
{
class UnitModel;

/// @brief This command build a building
class EntityBuildingCommand : public Command
{
public:
	EntityBuildingCommand(Handle const &commandHandle_p, Handle const &source_p, Handle const &target_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p, bool init_p=false);

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const override;

	virtual CommandData * newData() const override
	{
		return new MoveData(_subMoveCommand.getFinalPoint(), _subMoveCommand.getGridStatus(), _subMoveCommand.getWaypoints());
	}

private:
	Handle const _source;
	Handle const _target;

	/// @brief sub move command
	EntityMoveCommand _subMoveCommand;
};

} // namespace octopus


#endif
