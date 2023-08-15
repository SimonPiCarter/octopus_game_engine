#ifndef __StateRemoveConstraintPositionStep__
#define __StateRemoveConstraintPositionStep__

#include "step/Steppable.hh"
#include "utils/Fixed.hh"

namespace octopus
{
class StateRemoveConstraintPositionStep : public Steppable
{
public:
	StateRemoveConstraintPositionStep(unsigned long player_p, Fixed const &val_p, Fixed const &min_p, Fixed const &max_p, bool less_p, bool x_p)
		: _player(player_p), _val(val_p), _min(min_p), _max(max_p), _less(less_p), _x(x_p)
	{}

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

	/// @brief the player to apply the constraint to
	unsigned long const _player;
	/// @brief the value of the constraint
	Fixed const _val;
	/// @brief the min value where the constraint apply
	Fixed const _min;
	/// @brief the min value where the constraint apply
	Fixed const _max;
	/// @brief set if the constraint force entity < of the val given
	bool const _less;
	/// @brief set if the constraint enforces to x component
	bool const _x;
};

} // octopus

#endif
