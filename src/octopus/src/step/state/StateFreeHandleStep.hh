#ifndef __StateFreeHandleStep__
#define __StateFreeHandleStep__

#include "step/Steppable.hh"
#include "state/Handle.hh"

#include <list>

namespace octopus
{

class StateFreeHandleStep : public Steppable
{
public:
    StateFreeHandleStep(std::list<Handle> const &freeHandles_p)
		: _freeHandles(freeHandles_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override
	{
		return _freeHandles.empty();
	}
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}
private:
	std::list<Handle> _freeHandles;
};

}

#endif
