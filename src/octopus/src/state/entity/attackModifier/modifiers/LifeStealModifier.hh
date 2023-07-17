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
struct CommandContext;

/// @brief this class is aimed at lifestealing
class LifeStealModifier
{
public:
    LifeStealModifier(Fixed ratio_p) : _ratio(ratio_p) {}

	/// @brief create a new attack steppable
	void newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &target_p, State const &state_p, CommandContext const &commandContext_p, Step const &step_p, bool disableMainAttack_p=false) const;

    Fixed _ratio;
};

} // namespace octopus


#endif
