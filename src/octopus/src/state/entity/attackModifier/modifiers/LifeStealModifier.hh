#ifndef __LifeStealModifier__
#define __LifeStealModifier__

#include <vector>

#include "utils/Fixed.hh"
#include "state/entity/attackModifier/AttackModifierData.hh"

namespace octopus
{
class Entity;
class State;
class StepShallow;
class Steppable;

/// @brief this class is aimed at lifestealing
class LifeStealModifier
{
public:
    LifeStealModifier(Fixed ratio_p) : _ratio(ratio_p) {}

	/// @brief create a new attack steppable
	void newAttackSteppable(StepShallow &step_p, AttackModifierData const &data_p, State const &state_p, bool disableMainAttack_p=false) const;

    Fixed _ratio;
};

} // namespace octopus


#endif
