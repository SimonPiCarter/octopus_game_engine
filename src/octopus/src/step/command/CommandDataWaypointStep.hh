#ifndef __CommandDataWaypointStep__
#define __CommandDataWaypointStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
class CommandDataWaypointAddStep : public Steppable
{
public:
	CommandDataWaypointAddStep(Handle const &handle_p, Vector const &waypoint_p)
		: _handle(handle_p) , _waypoint(waypoint_p){}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override;

	Handle _handle {0};
	Vector _waypoint {0};
};

class CommandDataWaypointRemoveStep : public Steppable
{
public:
	CommandDataWaypointRemoveStep(Handle const &handle_p, Vector const &waypoint_p)
		: _handle(handle_p) , _waypoint(waypoint_p){}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override;

	Handle _handle {0};
	Vector _waypoint {0};
};
}

#endif
