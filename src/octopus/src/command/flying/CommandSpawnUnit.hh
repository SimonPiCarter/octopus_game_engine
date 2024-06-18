#ifndef __CommandSpawnUnit__
#define __CommandSpawnUnit__

#include "command/Command.hh"
#include "state/entity/Unit.hh"

namespace octopus
{

/// @brief this FlyingCommand is to be used to spawn units when getting handle is not possible
class CommandSpawnUnit : public FlyingCommand
{
public:
	CommandSpawnUnit(Unit const &unit_p)
		: FlyingCommand(), _unit(unit_p)
	{}

	/// @brief compile command or info into the step
	/// @return true if command is over
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const * data_p, PathManager &pathManager_p) const;

	/// @brief create data supporting the command actions
	virtual CommandData * newData() const { return nullptr; }

protected:
	/// @brief the unit to be spawned
	Unit const _unit;
};
} // namespace octopus

#endif
