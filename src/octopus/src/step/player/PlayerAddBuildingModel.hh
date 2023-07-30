#ifndef __PlayerAddBuildingModel__
#define __PlayerAddBuildingModel__

#include "state/entity/Entity.hh"
#include "state/State.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{

struct BuildingModel;

struct PlayerAddBuildingModelData : public SteppableData
{
    bool skipped = false;
};

class PlayerAddBuildingModel : public Steppable
{
public:
	PlayerAddBuildingModel(unsigned long idx_p, BuildingModel const &model_p) : _playerIdx(idx_p), _model(model_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override
	{
		return false;
	}
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}
    virtual SteppableData * newData(State const &state_p) const;

protected:
	unsigned long _playerIdx;
	BuildingModel const & _model;
};

}

#endif
