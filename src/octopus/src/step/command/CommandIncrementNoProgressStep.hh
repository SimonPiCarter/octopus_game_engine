#ifndef __CommandIncrementNoProgressStep__
#define __CommandIncrementNoProgressStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{
class EntityAttackCommand;

class CommandIncrementNoProgressStep : public Steppable
{
public:
	CommandIncrementNoProgressStep(Handle const &handle_p, unsigned long old_p, unsigned long new_p)
		: _handle(handle_p) , _old(old_p), _new(new_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override;

	Handle const _handle {0};
	unsigned long const _old;
	unsigned long const _new;
};

}

#endif
