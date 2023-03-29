#ifndef __TriggerEnableChange__
#define __TriggerEnableChange__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{

class TriggerEnableChange : public Steppable
{
public:
	TriggerEnableChange(Handle const &handleTrigger_p, bool old_p, bool new_p)
		: _handleTrigger(handleTrigger_p) , _old(old_p), _new(new_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handleTrigger {0};
	bool const _old {0};
	bool const _new {0};
};

} // namespace octopus


#endif
