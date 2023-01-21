#ifndef __UnitHarvestStep__
#define __UnitHarvestStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "state/ResourceType.hh"

namespace octopus
{

class UnitHarvestQuantityStep : public Steppable
{
public:
	UnitHarvestQuantityStep(Handle const &handle_p, Handle const &res_p, double const &qty_p)
		: _handle(handle_p) , _res(res_p), _qty(qty_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override;

	Handle _handle {0};
	Handle _res {0};
	double _qty {0};
};

class UnitHarvestTypeStep : public Steppable
{
public:
	UnitHarvestTypeStep(Handle const &handle_p, double oldVal_p, ResourceType old_p, ResourceType new_p)
		: _handle(handle_p) , _oldVal(oldVal_p), _old(old_p) , _new(new_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override;

	Handle _handle {0};
	double _oldVal {0.};
	ResourceType _old {ResourceType::Food};
	ResourceType _new {ResourceType::Food};
};

class UnitHarvestDropStep : public Steppable
{
public:
	UnitHarvestDropStep(Handle const &handle_p, double qty_p)
		: _handle(handle_p) , _qty(qty_p){}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override;

	Handle _handle {0};
	double _qty {0};
};

} // namespace octopus


#endif