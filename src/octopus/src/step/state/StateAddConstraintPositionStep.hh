#ifndef __StateAddConstraintPositionStep__
#define __StateAddConstraintPositionStep__

#include "step/Steppable.hh"
#include "utils/Fixed.hh"


namespace octopus
{
/// @brief spawn a position constraint
class StateAddConstraintPositionStep : public Steppable
{
public:
	/// @brief spawn a position constraint
	/// @param player_p the player that will be constrained
	/// @param val_p this is the value of the position that will be constrained (x or y)
	/// @param min_p this is the the min position (y or x) where the constraint will apply
	/// @param max_p this is the the max position (y or x) where the constraint will apply
	/// @param less_p tell if the constraint constraint the position to be less than the value of more
	/// @param x_p tells if the val_p is a x (if true) or y value (min and max will be the opposite)
	StateAddConstraintPositionStep(unsigned long player_p, Fixed const &val_p, Fixed const &min_p, Fixed const &max_p, bool less_p, bool x_p)
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
