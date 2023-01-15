#ifndef __EntityAttackMoveCommand__
#define __EntityAttackMoveCommand__

#include "Command.hh"
#include "EntityMoveCommand.hh"

#include "data/AttackMoveData.hh"
#include "state/Handle.hh"
#include "utils/Vector.hh"
#include <list>

namespace octopus
{

class Entity;

/// @brief This command will move along waypoints and will attack targets
/// that come into range while returning to its path once target are missing
/// or too far
class EntityAttackMoveCommand : public Command
{
public:
	EntityAttackMoveCommand(Handle const &commandHandle_p, Handle const &source_p, std::list<Vector> const &waypoints_p);

	/// @brief
	virtual bool applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const override;

	virtual CommandData * newData() const override { return new CommandDataWithData<AttackMoveData>({0, {}}); }

private:
	Handle const _source;
	std::list<Vector> const _waypoints;

	/// @brief sub move command
	EntityMoveCommand _subMoveCommand;
};

} // namespace octopus


#endif
