#ifndef __AttackModifier__
#define __AttackModifier__

namespace octopus
{
class Entity;
class State;
class Steppable;

/// @brief this class is aimed at representing different attack modifiers
/// dot/aoe/chaining for example
class AttackModifier
{
public:
	virtual ~AttackModifier();

	virtual Steppable * newAttackSteppable(const Entity &ent_p, const Entity &target_p, State const &state_p) const = 0;
};

/// @brief create attack steppable using eventual attack modifier
Steppable * newAttackSteppable(const Entity &ent_p, const Entity &target_p, State const &state_p);

} // namespace octopus


#endif
