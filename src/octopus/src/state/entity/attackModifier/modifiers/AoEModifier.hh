#ifndef __AoEModifier__
#define __AoEModifier__

#include "utils/Fixed.hh"
#include <vector>

namespace octopus
{
class Entity;
class State;
class Step;
class Steppable;

/// @brief this class is aimed at dealing aoe damage
class AoEModifier
{
public:
    AoEModifier(double ratio_p, Fixed const &range_p, bool friendlyFire_p=false) : _ratio(ratio_p), _range(range_p), _friendlyFire(friendlyFire_p) {}

	/// @brief create a new attack steppable
	void newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p, bool disableMainAttack_p=false) const;

    double _ratio;
    Fixed _range;
    bool _friendlyFire;
};

} // namespace octopus


#endif
