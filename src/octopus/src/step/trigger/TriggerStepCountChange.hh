#ifndef __TriggerStepCountChange__
#define __TriggerStepCountChange__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{

class TriggerStepCountChange : public Steppable
{
public:
	TriggerStepCountChange(Handle const &handleTrigger_p, Handle const &handleListener_p, unsigned long old_p, unsigned long new_p)
		: _handleTrigger(handleTrigger_p) , _handleListener(handleListener_p), _old(old_p), _new(new_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handleTrigger {0};
	Handle const _handleListener {0};
	unsigned long const _old {0};
	unsigned long const _new {0};
};

} // namespace octopus


#endif
