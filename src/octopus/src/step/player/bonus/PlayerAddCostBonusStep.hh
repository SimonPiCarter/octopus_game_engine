#ifndef __PlayerAddCostBonusStep__
#define __PlayerAddCostBonusStep__

#include <string>
#include "step/Steppable.hh"
#include "state/entity/buff/Buff.hh"

namespace octopus
{

struct PlayerCostBonusData : public SteppableData
{
    PlayerCostBonusData(Buff const &buff_p) : oldBuff(buff_p) {}
    // buff registered
	Buff oldBuff;
};

/// @brief add options for the given divinity types for the player
class PlayerAddCostBonusStep : public Steppable
{
public:
	PlayerAddCostBonusStep(unsigned long player_p, std::string const &model_p, std::string const &res_p, Buff const &buff_p)
		: _player(player_p), _model(model_p), _res(res_p), _buff(buff_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override { return false; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

    virtual SteppableData * newData(State const &) const;

	unsigned long const _player;
	std::string const _model;
	std::string const _res;
	Buff const _buff;
};

} // namespace octopus


#endif
