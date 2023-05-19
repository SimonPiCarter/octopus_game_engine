#ifndef __PlayerAnchorDivinityStep__
#define __PlayerAnchorDivinityStep__

#include <cmath>
#include <string>

#include "step/Steppable.hh"
#include "utils/Fixed.hh"

namespace octopus
{

/// @brief change anchor of the given divinity type for the player
class PlayerAnchorDivinityStep : public Steppable
{
public:
	PlayerAnchorDivinityStep(unsigned long player_p, std::string type_p, Fixed old_p, Fixed new_p)
		: _player(player_p), _type(type_p), _old(old_p), _new(new_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override { return numeric::abs(_old-_new) < 1e-3; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	unsigned long const _player {0};
	std::string const _type;
	Fixed const _old;
	Fixed const _new;
};

} // namespace octopus


#endif
