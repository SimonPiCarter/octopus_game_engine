#ifndef __ResourceSlotStep__
#define __ResourceSlotStep__

#include "state/Handle.hh"
#include "state/entity/Resource.hh"
#include "step/Steppable.hh"
#include "utils/Fixed.hh"

namespace octopus
{

struct ResourceSlotStepData : public SteppableData
{
	ResourceSlotStepData(HarvestPoint const &point_p) : old(point_p) {}
	HarvestPoint old;
};

class ResourceSlotStep : public Steppable
{
public:
	ResourceSlotStep(Handle const &res_p, int const &idx_p, Handle const &harvester_p, bool const &free_p)
		:  _res(res_p), _idx(idx_p), _harvester(harvester_p), _free(free_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	/// @brief create an optional new SteppableData
	/// @return nullptr if not required
	virtual SteppableData * newData(State const &state_p) const;

	/// @brief handle of the resource to update
	Handle const _res {0};
	/// @brief index of the slot to be updated
	int const _idx {0};
	/// @brief handle of the harvester
	Handle const _harvester {0};
	/// @brief true if we free the slot
	bool const _free {true};
};

} // namespace octopus


#endif
