#ifndef __PlayerLevelUpUpgradeStep__
#define __PlayerLevelUpUpgradeStep__

#include "step/Steppable.hh"
#include <string>

namespace octopus
{

/// @brief increase lvl of the given divinity type for the player
class PlayerLevelUpUpgradeStep : public Steppable
{
public:
	PlayerLevelUpUpgradeStep(unsigned long player_p, std::string type_p)
		: _player(player_p), _type(type_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override { return false; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	unsigned long const _player {0};
	std::string const _type;
};

} // namespace octopus


#endif
