#ifndef __BuildingRallyPointCommand__
#define __BuildingRallyPointCommand__

#include "command/Command.hh"

#include "command/data/ProductionData.hh"
#include "state/Handle.hh"
#include "utils/Vector.hh"

namespace octopus
{

/// @brief This command will cancel the given building
class BuildingRallyPointCommand : public Command
{
public:
	/// @brief Rally point command constructor to set the rallypoint
	BuildingRallyPointCommand(Handle const &handle_p, Vector const &rallyPoint_p, bool rallyPointEntityActive_p, Handle const &rallyPointEntity_p);
	/// @brief Rally point command constructor to reset the rallypoint
	BuildingRallyPointCommand(Handle const &handle_p);

	/// @brief we need to use register to register the step because otherwise it will override production
	virtual void registerCommand(Step & step_p, State const &state_p) override;

	virtual bool applyCommand(Step &, State const &, CommandData const *, PathManager &) const override { return true; }

	virtual CommandData * newData() const override { return nullptr; }

	bool const _reset;
	Vector const _rallyPoint;
	bool const _rallyPointEntityActive;
	Handle const _rallyPointEntity;
};

} // namespace octopus


#endif
