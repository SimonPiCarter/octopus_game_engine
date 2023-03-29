#ifndef __PlayerResetOptionDivinityStep__
#define __PlayerResetOptionDivinityStep__

#include <map>

#include "step/Steppable.hh"
#include "state/DivinityType.hh"
#include "state/player/DivinityOption.hh"

namespace octopus
{

/// @brief reset the divinity options of the given player
class PlayerResetOptionDivinityStep : public Steppable
{
public:
	PlayerResetOptionDivinityStep(unsigned long player_p, DivinityOption const option_p)
		: _player(player_p), _option(option_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override { return false; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	unsigned long const _player {0};

	DivinityOption const _option;
};

} // namespace octopus


#endif
