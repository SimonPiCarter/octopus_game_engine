#ifndef __LifeStealModifier__
#define __LifeStealModifier__

#include "utils/Fixed.hh"
#include <vector>

namespace octopus
{
class Entity;
class State;
class Step;
class Steppable;

/// @brief this class is aimed at lifestealing
class LifeStealModifier
{
public:
    LifeStealModifier(double ratio_p) : _ratio(ratio_p) {}

	/// @brief create a new attack steppable
	std::vector<Steppable *> newAttackSteppable(const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p) const;

    double _ratio;
};

} // namespace octopus


#endif
