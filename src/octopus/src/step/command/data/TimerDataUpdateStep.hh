#ifndef __TimerDataUpdateStep__
#define __TimerDataUpdateStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{
class TimerDataUpdateStep : public Steppable
{
public:
	TimerDataUpdateStep(Handle const &handle_p, unsigned long old_p, unsigned long new_p)
		: _handle(handle_p) , _old(old_p), _new(new_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handle {0};
	unsigned long const _old;
	unsigned long const _new;
};

}

#endif
