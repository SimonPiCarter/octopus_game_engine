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
	BuildingRallyPointCommand() {}
	/// @brief Rally point command constructor to set the rallypoint
	BuildingRallyPointCommand(Handle const &handle_p, Vector const &rallyPoint_p, bool rallyPointEntityActive_p, Handle const &rallyPointEntity_p);
	/// @brief Rally point command constructor to reset the rallypoint
	BuildingRallyPointCommand(Handle const &handle_p);

	/// @brief we need to use register to register the step because otherwise it will override production
	virtual void registerCommand(Step & step_p, State const &state_p) override;

	virtual bool applyCommand(StepShallow &, State const &, CommandData const *, PathManager &) const override { return true; }

	bool _reset {false};
	Vector _rallyPoint {0,0};
	bool _rallyPointEntityActive {false};
	Handle _rallyPointEntity {0};
};

} // namespace octopus


#endif
