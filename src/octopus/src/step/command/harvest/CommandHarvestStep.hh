#ifndef __CommandHarvestStep__
#define __CommandHarvestStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Fixed.hh"

namespace octopus
{

class CommandResourceChangeStep : public Steppable
{
public:
	CommandResourceChangeStep(Handle const &handle_p, Handle const &old_p, Handle const &new_p)
		: _handle(handle_p) , _oldResource(old_p), _newResource(new_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

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
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle _handle {0};
	Handle _oldDeposit {0};
	Handle _newDeposit {0};
};

class CommandHarvestingChangeStep : public Steppable
{
public:
	CommandHarvestingChangeStep(Handle const &handle_p, bool old_p, bool new_p)
		: _handle(handle_p) , _old(old_p), _new(new_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle _handle {0};
	bool _old {0};
	bool _new {0};
};

class CommandHarvestTimeSinceHarvestStep : public Steppable
{
public:
	/// @brief dropped is the qty buffed
	CommandHarvestTimeSinceHarvestStep(Handle const &handle_p, Fixed old_p, Fixed new_p)
		: _handle(handle_p), _old(old_p), _new(new_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handle {0};
	Fixed const _old {0};
	Fixed const _new {0};
};

class CommandHarvestPointChangeStep : public Steppable
{
public:
	/// @brief update the harvest point
	CommandHarvestPointChangeStep(Handle const &handle_p, Handle const &res_p, int old_p, int new_p)
		: _handle(handle_p), _res(res_p), _old(old_p), _new(new_p) {}

	virtual void consolidate(State const &state_p, Step const &step_p) override;

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handle {0};
	Handle const _res {0};
	int const _old {0};
	// non const because it can be changed in consolidate to -1 if slot is already taken
	int _new {0};
};

} // namespace octopus


#endif
