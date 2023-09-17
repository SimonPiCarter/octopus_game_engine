#ifndef __ChainingModifier__
#define __ChainingModifier__

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
class ChainingModifier
{
public:
    ChainingModifier(unsigned long delay_p, unsigned long nbOfTicks_p, Fixed ratio_p, Fixed range_p)
        : _delay(delay_p), _nbOfTicks(nbOfTicks_p), _ratio(ratio_p), _range(range_p) {}

	/// @brief create a new attack steppable
	void newAttackSteppable(Step &step_p, AttackModifierData const &data_p, State const &state_p, bool disableMainAttack_p=false) const;

    unsigned long _delay;
    unsigned long _nbOfTicks;
    Fixed _ratio;
    Fixed _range;
};

} // namespace octopus


#endif
