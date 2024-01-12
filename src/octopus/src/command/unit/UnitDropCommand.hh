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
	UnitDropCommand() {}
	UnitDropCommand(Handle const &commandHandle_p, Handle const &source_p, Handle const &deposit_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p, bool init_p=false);

	virtual void registerCommand(Step & step_p, State const &state_p);

	/// @brief
	virtual bool applyCommand(StepShallow & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

	virtual CommandData * getData() override { return _subMoveCommand.getData(); }
	virtual CommandData const * getData() const override { return _subMoveCommand.getData(); }

	virtual void cleanUp(StepShallow & step_p, State const &state_p, CommandData const *data_p) const override
	{
		_subMoveCommand.cleanUp(step_p, state_p, data_p);
	}

	Handle const &getDeposit() const { return _deposit; }
	EntityMoveCommand const &getSubMoveCommand() const { return _subMoveCommand; }
private:
	Handle _source {0};
	Handle _deposit {0};

	/// @brief sub move command
	EntityMoveCommand _subMoveCommand;
};

} // namespace octopus


#endif
