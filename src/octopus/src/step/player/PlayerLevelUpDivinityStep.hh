#ifndef __PlayerLevelUpDivinityStep__
#define __PlayerLevelUpDivinityStep__

#include "step/Steppable.hh"
#include "state/DivinityType.hh"

namespace octopus
{

/// @brief increase lvl of the given divinity type for the player
class PlayerLevelUpDivinityStep : public Steppable
{
public:
	PlayerLevelUpDivinityStep(unsigned long player_p, DivinityType type_p)
		: _player(player_p), _type(type_p) {}

	virtual void apply(State &state_p, SteppableData *) const override;
	virtual void revert(State &state_p, SteppableData *) const override;

	virtual bool isNoOp() const override { return false; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	unsigned long const _player {0};
	DivinityType const _type;
};

} // namespace octopus


#endif
