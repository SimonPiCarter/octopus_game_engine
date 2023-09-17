#ifndef __DotModifier__
#define __DotModifier__

#include <vector>

#include "utils/Fixed.hh"
#include "state/entity/attackModifier/AttackModifierData.hh"

namespace octopus
{
class Entity;
class State;
class Step;
class Steppable;

/// @brief this class is aimed at lifestealing
class DotModifier
{
public:
    DotModifier(unsigned long tickRate_p, unsigned long nbOfTicks_p, Fixed dmg_p) : _tickRate(tickRate_p), _nbOfTicks(nbOfTicks_p), _dmg(dmg_p) {}

	/// @brief create a new attack steppable
	void newAttackSteppable(Step &step_p, AttackModifierData const &data_p, State const &state_p, bool disableMainAttack_p=false) const;

    unsigned long _tickRate;
    unsigned long _nbOfTicks;
    Fixed _dmg;
};

} // namespace octopus


#endif
