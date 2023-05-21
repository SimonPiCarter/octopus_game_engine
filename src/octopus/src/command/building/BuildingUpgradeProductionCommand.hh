#ifndef __BuildingUpgradeProductionCommand__
#define __BuildingUpgradeProductionCommand__

#include "command/Command.hh"

#include "command/data/ProductionData.hh"
#include "state/Handle.hh"

namespace octopus
{
struct Upgrade;

/// @brief This command will produce a unit according to the given model
class BuildingUpgradeProductionCommand : public Command
{
public:
	BuildingUpgradeProductionCommand() {}
	BuildingUpgradeProductionCommand(Handle const &commandHandle_p, Handle const &source_p, Upgrade const &upgrade_p);

	/// @brief register the command into the step
	/// This method is responsible to
	/// handle cost of command and spawning command in step
	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

	virtual CommandData * getData() override { return &_data; }
	virtual CommandData const * getData() const override { return &_data; }

	Upgrade const &getUpgrade() const { return *_upgrade; }

private:
	Handle _source {0};

	Upgrade const *_upgrade {nullptr};

	UpgradeProductionData _data;
};

} // namespace octopus


#endif
