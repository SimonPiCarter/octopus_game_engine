#ifndef __TriggerEntityAddStep__
#define __TriggerEntityAddStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{

class Entity;

class TriggerEntityAddStep : public Steppable
{
public:
	TriggerEntityAddStep(Handle const &handleTrigger_p, Handle const &handleListener_p, Entity const *ent_p)
		: _handleTrigger(handleTrigger_p) , _handleListener(handleListener_p), _ent(ent_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override { return _ent == nullptr; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handleTrigger {0};
	Handle const _handleListener {0};
	Entity const * const _ent {nullptr};
};

} // namespace octopus


#endif
