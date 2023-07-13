#ifndef __BuildingCancelCommand__
#define __BuildingCancelCommand__

#include "command/Command.hh"

#include "command/data/ProductionData.hh"
#include "state/Handle.hh"

namespace octopus
{

/// @brief This command will cancel the given building
class BuildingCancelCommand : public Command
{
public:
	BuildingCancelCommand() {}
	BuildingCancelCommand(Handle const &commandHandle_p) : Command(commandHandle_p) {}

	/// @brief register the command into the step
	/// This method is responsible to
	/// handle cost of command and spawning command in step
	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief
	virtual bool applyCommand(Step & , State const &, CommandData const *, CommandContext &) const override { return true; }
};

} // namespace octopus


#endif
