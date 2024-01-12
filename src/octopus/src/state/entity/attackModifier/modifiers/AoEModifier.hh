#ifndef __AoEModifier__
#define __AoEModifier__

#include <vector>

#include "utils/Fixed.hh"
#include "state/entity/attackModifier/AttackModifierData.hh"

namespace octopus
{
class Entity;
class State;
class StepShallow;
class Steppable;

/// @brief this class is aimed at dealing aoe damage
class AoEModifier
{
public:
    AoEModifier(Fixed ratio_p, Fixed const &range_p, bool friendlyFire_p=false) : _ratio(ratio_p), _range(range_p), _friendlyFire(friendlyFire_p) {}

	/// @brief create a new attack steppable
	void newAttackSteppable(StepShallow &step_p, AttackModifierData const &data_p, State const &state_p, bool disableMainAttack_p=false) const;

    Fixed _ratio;
    Fixed _range;
    bool _friendlyFire;
};

} // namespace octopus


#endif
