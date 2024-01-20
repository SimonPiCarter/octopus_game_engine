#ifndef __UnitDataStep__
#define __UnitDataStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "state/entity/Unit.hh"

namespace octopus
{
struct UnitDataStepData : public SteppableData
{
	UnitDataStepData(UnitData const * data_p) : _data(data_p?data_p->clone():nullptr) {}
	~UnitDataStepData() { delete _data; }
	UnitData const * const _data;
};

class UnitDataStep : public Steppable
{
public:
	UnitDataStep(Handle const &handle_p, UnitData const *data_p)
		: _handle(handle_p) , _data(data_p) {}
	~UnitDataStep() { delete _data; }

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *data_p) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	SteppableData * newData(State const &state_p) const;

	Handle const _handle {0};
	UnitData const * const _data = nullptr;
};

} // namespace octopus


#endif
