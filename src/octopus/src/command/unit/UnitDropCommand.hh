#ifndef __UnitDropCommand__
#define __UnitDropCommand__

#include "command/Command.hh"
#include "state/Handle.hh"
#include "command/data/HarvestMoveData.hh"
#include "command/entity/EntityMoveCommand.hh"

namespace octopus
{

/// @brief Command that make the unit drop resources to the deposit
class UnitDropCommand : public Command
{
public:
	UnitDropCommand(Handle const &commandHandle_p, Handle const &source_p, Handle const &deposit_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p, bool init_p=false);

	virtual void registerCommand(Step & step_p, State const &state_p);

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

	virtual CommandData * newData() const override
	{
		return new MoveData(_subMoveCommand.getFinalPoint(), _subMoveCommand.getGridStatus(), _subMoveCommand.getWaypoints());
	}

	virtual void cleanUp(Step & step_p, State const &state_p, CommandData const *data_p) const override
	{
		_subMoveCommand.cleanUp(step_p, state_p, data_p);
	}
private:
	Handle const _source;
	Handle const _deposit;

	/// @brief sub move command
	EntityMoveCommand _subMoveCommand;
};

} // namespace octopus


#endif
