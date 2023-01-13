#ifndef __EntityMoveCommand__
#define __EntityMoveCommand__

#include "Command.hh"

#include "state/Handle.hh"
#include "utils/Vector.hh"
#include <list>


namespace octopus
{

/// @brief Basic class that ensure move command with waypoints or not
/// Entity will be moved along all waypoints and command will terminate
/// when last one is reached
class EntityMoveCommand : public Command
{
public:
	EntityMoveCommand(Handle const &commandHandle_p, Handle const &source_p, std::list<Vector> waypoints_p);

	/// @brief check if _ent is close to the waypoint, if so
	/// remove reached waypoint and create a step to the next waypoint
	/// multiple waypoint may be removed at once last waypoint
	/// is removed terminate
	virtual bool applyCommand(Step & step_p, State const &state_p) override;
private:
	Handle const _source;
	std::list<Vector> _waypoints;
};

} // namespace octopus


#endif
