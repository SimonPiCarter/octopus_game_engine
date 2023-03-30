#ifndef __ChainingModifier__
#define __ChainingModifier__

#include "utils/Fixed.hh"
#include <vector>

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
    ChainingModifier(unsigned long delay_p, unsigned long nbOfTicks_p, double ratio_p, double range_p)
        : _delay(delay_p), _nbOfTicks(nbOfTicks_p), _ratio(ratio_p), _range(range_p) {}

	/// @brief create a new attack steppable
	std::vector<Steppable *> newAttackSteppable(const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p) const;

    unsigned long _delay;
    unsigned long _nbOfTicks;
    double _ratio;
    double _range;
};

} // namespace octopus


#endif