#ifndef __PlayerAnchorDivinityStep__
#define __PlayerAnchorDivinityStep__

#include <cmath>

#include "step/Steppable.hh"
#include "state/DivinityType.hh"

namespace octopus
{

/// @brief change anchor of the given divinity type for the player
class PlayerAnchorDivinityStep : public Steppable
{
public:
	PlayerAnchorDivinityStep(unsigned long player_p, DivinityType type_p, double delta_p)
		: _player(player_p), _type(type_p), _delta(delta_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p) const override;

	virtual bool isNoOp() const override { return std::abs(_delta) < 1e-3; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	unsigned long const _player {0};
	DivinityType const _type;
	double const _delta;
};

} // namespace octopus


#endif
