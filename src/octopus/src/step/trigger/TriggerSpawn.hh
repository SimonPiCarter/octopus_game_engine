#ifndef __TriggerSpawn__
#define __TriggerSpawn__

#include "step/Steppable.hh"

namespace octopus
{

class Trigger;

class TriggerSpawn : public Steppable
{
public:
	TriggerSpawn(Trigger *trigger_p)
		: _trigger(trigger_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Trigger const *_trigger;
};

} // namespace octopus


#endif
