#ifndef __PlayerBuffAllStep__
#define __PlayerBuffAllStep__

#include <vector>
#include <string>

#include "state/entity/buff/Buff.hh"
#include "step/Steppable.hh"
#include "step/entity/buff/EntityBuffStep.hh"

namespace octopus
{

struct PlayerBuffAllStepData : public SteppableData
{
	~PlayerBuffAllStepData();
    std::vector<SteppableData const *> _buffStepsData;
};

/// @brief register buff of player and add buff to all living entities
class PlayerBuffAllStep : public Steppable
{
public:
	PlayerBuffAllStep(unsigned long player_p, TimedBuff const &buff_p, std::string const &model_p)
		: _player(player_p), _buff(buff_p), _model(model_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *data_p) const override;

	virtual bool isNoOp() const override { return false; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

    virtual SteppableData * newData(State const &state_p) const;

	unsigned long const _player;
	TimedBuff const _buff;
    std::string const _model;
};

} // namespace octopus


#endif
