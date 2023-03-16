#ifndef __CommandDataWaypointStep__
#define __CommandDataWaypointStep__

#include <list>
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

	virtual void apply(State &state_p, SteppableData *) const override;
	virtual void revert(State &state_p, SteppableData *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle _handle {0};
	Vector _waypoint;
};

class CommandDataWaypointRemoveStep : public Steppable
{
public:
	CommandDataWaypointRemoveStep(Handle const &handle_p, Vector const &waypoint_p)
		: _handle(handle_p) , _waypoint(waypoint_p){}

	virtual void apply(State &state_p, SteppableData *) const override;
	virtual void revert(State &state_p, SteppableData *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle _handle {0};
	Vector _waypoint;
};

class CommandDataWaypointSetStep : public Steppable
{
public:
	CommandDataWaypointSetStep(Handle const &handle_p, std::list<Vector> const &old_p, std::list<Vector> const &new_p)
		: _handle(handle_p) , _old(old_p), _new(new_p) {}

	virtual void apply(State &state_p, SteppableData *) const override;
	virtual void revert(State &state_p, SteppableData *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle _handle {0};
	std::list<Vector> const _old;
	std::list<Vector> const _new;
};

}

#endif
