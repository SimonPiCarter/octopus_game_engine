#ifndef __PlayerProducedUpgradeStep__
#define __PlayerProducedUpgradeStep__

#include <map>

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Fixed.hh"

namespace octopus
{

struct PlayerProducedUpgradeData: public SteppableData
{
	/// @brief true if the step will change the state
	bool needChange {false};
};

class PlayerProducedUpgradeStep : public Steppable
{
public:
	PlayerProducedUpgradeStep(unsigned long player_p, std::string const &upgrade_p, bool add_p)
		: _player(player_p), _upgrade(upgrade_p), _add(add_p) {}

	virtual void apply(State &state_p) const override;
	virtual void revert(State &state_p, SteppableData const *) const override;

	virtual bool isNoOp() const override { return false; }
	virtual void visit(SteppableVisitor * visitor_p) const override
	{
		visitor_p->visit(this);
	}

	virtual SteppableData * newData(State const &) const;

	unsigned long _player {0};
	std::string _upgrade;
    bool _add {true};
};

} // namespace octopus


#endif
