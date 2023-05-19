#ifndef __UnitHarvestStep__
#define __UnitHarvestStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Fixed.hh"

namespace octopus
{

class UnitHarvestQuantityStep : public Steppable
{
public:
	UnitHarvestQuantityStep(Handle const &handle_p, Handle const &res_p, Fixed const &qty_p)
		: _handle(handle_p) , _res(res_p), _qty(qty_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handle {0};
	Handle const _res {0};
	Fixed const _qty {0};
};

class UnitHarvestTypeStep : public Steppable
{
public:
	UnitHarvestTypeStep(Handle const &handle_p, Fixed oldVal_p, std::string old_p, std::string new_p)
		: _handle(handle_p) , _oldVal(oldVal_p), _old(old_p) , _new(new_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handle {0};
	Fixed const _oldVal {0.};
	std::string const _old {"Food"};
	std::string const _new {"Food"};
};

class UnitHarvestDropStep : public Steppable
{
public:
	/// @brief dropped is the qty buffed
	UnitHarvestDropStep(Handle const &handle_p, Fixed qty_p, Fixed dropped_p)
		: _handle(handle_p), _qty(qty_p), _dropped(dropped_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	Handle const _handle {0};
	Fixed const _qty {0};
	Fixed const _dropped {0};
};

} // namespace octopus


#endif
