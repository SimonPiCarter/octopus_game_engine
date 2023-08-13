#ifndef __CommandWindUpDiffStep__
#define __CommandWindUpDiffStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{
class CommandWindUpDiffStep : public Steppable
{
public:
	CommandWindUpDiffStep(Handle const &handle_p, long diff_p)
		: _handle(handle_p) , _diff(diff_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handle {0};
	long const _diff {0};
};
}

#endif
