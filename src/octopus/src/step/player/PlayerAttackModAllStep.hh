#ifndef __PlayerAttackModAllStep__
#define __PlayerAttackModAllStep__

#include <vector>
#include <string>

#include "state/entity/attackModifier/AttackModifier.hh"
#include "step/Steppable.hh"
#include "step/entity/modifier/AttackModifierStep.hh"

namespace octopus
{

struct PlayerAttackModAllStepData : public SteppableData
{
	~PlayerAttackModAllStepData();
	AttackModifier _old;
    std::vector<SteppableData const *> _stepsData;
};

/// @brief register buff of player and add buff to all living entities
class PlayerAttackModAllStep : public Steppable
{
public:
	PlayerAttackModAllStep(unsigned long player_p, AttackModifier const &mod_p, std::string const &model_p)
		: _player(player_p), _mod(mod_p), _model(model_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *data_p) const override;

	virtual bool isNoOp() const override { return false; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

    virtual SteppableData * newData(State const &state_p) const;

	unsigned long const _player;
	AttackModifier const _mod;
    std::string const _model;
};

} // namespace octopus


#endif
