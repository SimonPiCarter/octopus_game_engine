#ifndef __BuildingUnitCancelCommand__
#define __BuildingUnitCancelCommand__

#include "command/Command.hh"

#include "command/data/ProductionData.hh"
#include "state/Handle.hh"

namespace octopus
{

/// @brief This command will produce a unit according to the given model
class BuildingUnitCancelCommand : public Command
{
public:
	BuildingUnitCancelCommand(Handle const &commandHandle_p, unsigned long idx_p) : Command(commandHandle_p), _idx(idx_p) {}

	/// @brief register the command into the step
	/// This method is responsible to
	/// handle cost of command and spawning command in step
	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief
	virtual bool applyCommand(Step & , State const &, CommandData const *, PathManager &) const override { return true; }

	virtual CommandData * newData() const override { return nullptr; }

	unsigned long const _idx;
};

} // namespace octopus


#endif