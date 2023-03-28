#ifndef __BuildingUnitProductionCommand__
#define __BuildingUnitProductionCommand__

#include "command/Command.hh"

#include "command/data/ProductionData.hh"
#include "state/Handle.hh"

namespace octopus
{
struct UnitModel;;

/// @brief This command will produce a unit according to the given model
class BuildingUnitProductionCommand : public Command
{
public:
	BuildingUnitProductionCommand(Handle const &commandHandle_p, Handle const &source_p, UnitModel const &model_p);

	/// @brief register the command into the step
	/// This method is responsible to
	/// handle cost of command and spawning command in step
	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const override;

	virtual CommandData * newData() const override { return new UnitProductionData(_model); }

	UnitModel const &getModel() const { return _model; }

private:
	Handle const _source;

	UnitModel const &_model;
};



/// @brief This command will produce a unit according to the given model
class BuildingUnitCancelCommand : public Command
{
public:
	BuildingUnitCancelCommand(Handle const &commandHandle_p, size_t idx_p) : Command(commandHandle_p), _idx(idx_p) {}

	/// @brief register the command into the step
	/// This method is responsible to
	/// handle cost of command and spawning command in step
	virtual void registerCommand(Step & step_p, State const &state_p) override;

	/// @brief
	virtual bool applyCommand(Step & , State const &, CommandData const *, PathManager &) const override { return true; }

	virtual CommandData * newData() const override { return nullptr; }

private:
	size_t const _idx;
};

} // namespace octopus


#endif
