#ifndef __EntityWaitCommand__
#define __EntityWaitCommand__

#include "Command.hh"
#include "entity/Entity.hh"


namespace octopus
{

/// @brief Basic class that ensure move command with waypoints or not
/// Entity will be moved along all waypoints and command will terminate
/// when last one is reached
class EntityWaitCommand : public Command
{
public:
	EntityWaitCommand(Entity &ent_p);

	/// @brief check if _ent is close to the waypoint, if so
	/// remove reached waypoint and create a step to the next waypoint
	/// multiple waypoint may be removed at once last waypoint
	/// is removed terminate
	virtual bool registerCommand(Step & step_p) override;
private:
	Entity & _ent;
};

} // namespace octopus


#endif
