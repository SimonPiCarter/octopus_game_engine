#ifndef __BuildingCommand__
#define __BuildingCommand__

#include "command/Command.hh"
#include "command/data/MoveData.hh"
#include "EntityMoveCommand.hh"
#include "state/Handle.hh"

namespace octopus
{
struct UnitModel;;

/// @brief This command build a building
class EntityBuildingCommand : public Command
{
public:
	EntityBuildingCommand() {}
	EntityBuildingCommand(Handle const &commandHandle_p, Handle const &source_p, Handle const &target_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p, bool init_p=false);

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

	virtual CommandData * getData() override { return _subMoveCommand.getData(); }
	virtual CommandData const * getData() const override { return _subMoveCommand.getData(); }

	virtual void cleanUp(Step & step_p, State const &state_p, CommandData const *data_p) const override
	{
		_subMoveCommand.cleanUp(step_p, state_p, data_p);
	}


	Handle getTarget() const { return _target; }
	EntityMoveCommand const &getSubMoveCommand() const { return _subMoveCommand; }

private:
	Handle _source {0};
	Handle _target {0};

	/// @brief sub move command
	EntityMoveCommand _subMoveCommand;
};

} // namespace octopus


#endif
