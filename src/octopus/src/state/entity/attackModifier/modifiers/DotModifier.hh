#ifndef __DotModifier__
#define __DotModifier__

#include "utils/Fixed.hh"
#include <vector>

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
    DotModifier(unsigned long tickRate_p, unsigned long nbOfTicks_p, double dmg_p) : _tickRate(tickRate_p), _nbOfTicks(nbOfTicks_p), _dmg(dmg_p) {}

	/// @brief create a new attack steppable
	void newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p, bool disableMainAttack_p=false) const;

    unsigned long _tickRate;
    unsigned long _nbOfTicks;
    double _dmg;
};

} // namespace octopus


#endif
