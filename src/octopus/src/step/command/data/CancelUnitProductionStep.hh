#ifndef __CancelUnitProductionStep__
#define __CancelUnitProductionStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{
class UnitProductionData;

class CancelUnitProductionStep : public Steppable
{
public:
	CancelUnitProductionStep(Handle const &handle_p, unsigned long const idx_p)
		: _handle(handle_p) , _idx(idx_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override { return false; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handle {0};
	unsigned long const _idx;
};

}

#endif
