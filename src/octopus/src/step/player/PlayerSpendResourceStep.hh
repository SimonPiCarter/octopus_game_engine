#ifndef __PlayerSpendResourceStep__
#define __PlayerSpendResourceStep__

#include <map>

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Fixed.hh"

namespace octopus
{

class PlayerSpendResourceStep : public Steppable
{
public:
	/// @brief dropped is the qty buffed
	PlayerSpendResourceStep(unsigned long player_p, std::map<std::string, Fixed> const & delta_p)
		: _player(player_p), _delta(delta_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override;
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	unsigned long _player {0};
	std::map<std::string, Fixed> _delta;
};

} // namespace octopus


#endif
