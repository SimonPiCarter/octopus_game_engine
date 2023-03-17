#ifndef __CommandNewTargetStep__
#define __CommandNewTargetStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{
class EntityAttackCommand;

class CommandNewTargetStep : public Steppable
{
public:
	CommandNewTargetStep(Handle const &handle_p, Handle const & new_p, Handle const & old_p)
		: _handle(handle_p) , _newTarget(new_p) , _oldTarget(old_p) {}

	virtual void apply(State &state_p, SteppableData *) const override;
	virtual void revert(State &state_p, SteppableData *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handle {0};
	Handle const _newTarget;
	Handle const _oldTarget;
};

}

#endif
