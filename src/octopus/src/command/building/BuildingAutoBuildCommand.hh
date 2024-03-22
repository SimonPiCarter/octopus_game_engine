#ifndef __BuildingAutoBuildCommand__
#define __BuildingAutoBuildCommand__

#include "command/Command.hh"

#include "command/data/ProductionData.hh"
#include "state/Handle.hh"

namespace octopus
{
struct UnitModel;;

/// @brief This command will set the the auto build model of the building
class BuildingAutoBuildCommand : public Command
{
public:
	BuildingAutoBuildCommand() {}
	BuildingAutoBuildCommand(Handle const &source_p, UnitModel const &model_p);

	/// @brief register the command into the step
	/// This method is responsible to
	/// handle cost of command and spawning command in step
	void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief
	bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

private:
	Handle _source {0};

	UnitModel const *_model {nullptr};
};

} // namespace octopus


#endif
