#ifndef __AttackModifier__
#define __AttackModifier__

namespace octopus
{
class Entity;
class State;
class Step;
class Steppable;

/// @brief this class is aimed at representing different attack modifiers
/// dot/aoe/chaining for example
class AttackModifier
{
public:
	virtual ~AttackModifier();

	/// @brief create a new attack steppable
	/// @param currentTargetHp_p the hp of the target with deduction of every hp change in the current step (not applied yet to the state)
	virtual Steppable * newAttackSteppable(const Entity &ent_p, const Entity &target_p, State const &state_p, double currentTargetHp_p, double targetHpMax_p) const = 0;
};

/// @brief create attack steppable using eventual attack modifier
	/// @param currentTargetHp_p the hp of the target with deduction of every hp change in the current step (not applied yet to the state)
Steppable * newAttackSteppable(const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p);

} // namespace octopus


#endif
