#ifndef __PlayerConditionalBuffAllStep__
#define __PlayerConditionalBuffAllStep__

#include <vector>
#include <string>

#include "state/entity/buff/ConditionalBuff.hh"
#include "step/Steppable.hh"
#include "step/entity/buff/EntityBuffStep.hh"

namespace octopus
{

/// @brief register buff of player and add buff to all living entities
class PlayerConditionalBuffAllStep : public Steppable
{
public:
	PlayerConditionalBuffAllStep(unsigned long player_p, ConditionalBuff const &buff_p, std::string const &model_p)
		: _player(player_p), _buff(buff_p), _model(model_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *data_p) const override;

	virtual bool isNoOp() const override { return false; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	unsigned long const _player;
	ConditionalBuff const _buff;
    std::string const _model;
};

} // namespace octopus


#endif
