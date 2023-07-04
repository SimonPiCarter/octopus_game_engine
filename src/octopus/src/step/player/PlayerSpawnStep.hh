#ifndef __SPAWN_PLAYER_STEP__
#define __SPAWN_PLAYER_STEP__

#include "state/entity/Entity.hh"
#include "state/State.hh"
#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Vector.hh"

namespace octopus
{

struct BuildingModel;

class PlayerSpawnStep : public Steppable
{
public:
	PlayerSpawnStep(unsigned long idx_p, unsigned long team_p) : _playerIdx(idx_p), _playerTeam(team_p) {}

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

	unsigned long getPlayerIdx() const { return _playerIdx; }
protected:
	unsigned long _playerIdx;
	unsigned long _playerTeam;
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
protected:
	unsigned long _playerIdx;
	BuildingModel const & _model;
};

}

#endif
