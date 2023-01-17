#ifndef __CommandHarvestStep__
#define __CommandHarvestStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "state/ResourceType.hh"

namespace octopus
{

class CommandResourceChangeStep : public Steppable
{
public:
	CommandResourceChangeStep(Handle const &handle_p, Handle const &old_p, Handle const &new_p)
		: _handle(handle_p) , _oldResource(old_p), _newResource(new_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override;

	Handle _handle {0};
	Handle _oldResource {0};
	Handle _newResource {0};
};

class CommandDepositChangeStep : public Steppable
{
public:
	CommandDepositChangeStep(Handle const &handle_p, Handle const &old_p, Handle const &new_p)
		: _handle(handle_p) , _oldDeposit(old_p), _newDeposit(new_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override;

	Handle _handle {0};
	Handle _oldDeposit {0};
	Handle _newDeposit {0};
};

} // namespace octopus


#endif
