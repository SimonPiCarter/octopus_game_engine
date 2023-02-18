#ifndef __TriggerEntityResetStep__
#define __TriggerEntityResetStep__

#include <vector>

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{

class Entity;

class TriggerEntityResetStep : public Steppable
{
public:
	TriggerEntityResetStep(Handle const &handleTrigger_p, Handle const &handleListener_p, std::vector<Entity const *> const ent_p)
		: _handleTrigger(handleTrigger_p) , _handleListener(handleListener_p), _ent(ent_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override { return _ent.empty(); }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handleTrigger {0};
	Handle const _handleListener {0};
	std::vector<Entity const *> _ent;
};

} // namespace octopus


#endif
