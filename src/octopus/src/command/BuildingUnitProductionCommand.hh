#ifndef __BuildingUnitProductionCommand__
#define __BuildingUnitProductionCommand__

#include "Command.hh"

#include "data/ProductionData.hh"
#include "state/Handle.hh"

namespace octopus
{
class UnitModel;

/// @brief This command will produce a unit according to the given model
class BuildingUnitProductionCommand : public Command
{
public:
	BuildingUnitProductionCommand(Handle const &commandHandle_p, Handle const &source_p, UnitModel const &model_p);

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const override;

	virtual CommandData * newData() const override { return new UnitProductionData(_model); }

private:
	Handle const _source;

	UnitModel const &_model;
};

} // namespace octopus


#endif
