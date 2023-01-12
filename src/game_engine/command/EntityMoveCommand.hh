#ifndef __EntityMoveCommand__
#define __EntityMoveCommand__

#include "entity/Entity.hh"
#include "entity/Vector.hh"
#include <list>


namespace game_engine
{
class Step;

class Command
{
public:
	/// @brief register new commands or info into the step
	/// @return true if command is over
	virtual bool registerCommand(Step & step_p) = 0;
};

/// @brief Basic class that ensure move command with waypoints or not
/// Entity will be moved along all waypoints and command will terminate
/// when last one is reached
class EntityMoveCommand : public Command
{
public:
	EntityMoveCommand(Entity &ent_p, std::list<Vector> waypoints_p);

	/// @brief check if _ent is close to the waypoint, if so
	/// remove reached waypoint and create a step to the next waypoint
	/// multiple waypoint may be removed at once last waypoint
	/// is removed terminate
	virtual bool registerCommand(Step & step_p) override;
private:
	Entity & _ent;
	std::list<Vector> _waypoints;
};

} // namespace game_engine


#endif
