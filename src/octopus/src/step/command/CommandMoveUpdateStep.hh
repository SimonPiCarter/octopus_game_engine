#ifndef __CommandMoveUpdateStep__
#define __CommandMoveUpdateStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{
class CommandMoveUpdateStep : public Steppable
{
public:
	CommandMoveUpdateStep(Handle const &handle_p, unsigned long oldTimeSinceUpdate_p, unsigned long oldGridStatus_p, unsigned long newGridStatus_p)
		: _handle(handle_p) , _old(oldTimeSinceUpdate_p), _oldGridStatus(oldGridStatus_p), _newGridStatus(newGridStatus_p){}

	virtual void apply(State &state_p, SteppableData *) const override;
	virtual void revert(State &state_p, SteppableData *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handle;
	unsigned long const _old;
	unsigned long const _oldGridStatus;
	unsigned long const _newGridStatus;
};


class CommandMoveStepSinceUpdateIncrementStep : public Steppable
{
public:
	CommandMoveStepSinceUpdateIncrementStep(Handle const &handle_p)
		: _handle(handle_p) {}

	virtual void apply(State &state_p, SteppableData *) const override;
	virtual void revert(State &state_p, SteppableData *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handle;
};

}

#endif
