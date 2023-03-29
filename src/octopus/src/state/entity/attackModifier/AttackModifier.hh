#ifndef __AttackModifier__
#define __AttackModifier__

#include <vector>
#include <variant>

#include "modifiers/AoEModifier.hh"
#include "modifiers/ChainingModifier.hh"
#include "modifiers/DotModifier.hh"
#include "modifiers/LifeStealModifier.hh"

namespace octopus
{
class Entity;
class State;
class Step;
class Steppable;

/// @brief this class is aimed at dealing aoe damage
class NoModifier
{
public:
	/// @brief create a new attack steppable
	std::vector<Steppable *> newAttackSteppable(const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p) const;
};

using AttackModifier = std::variant<
	NoModifier,
	AoEModifier,
	ChainingModifier,
	DotModifier,
	LifeStealModifier
>;

/// @brief create attack steppable using eventual attack modifier
	/// @param currentTargetHp_p the hp of the target with deduction of every hp change in the current step (not applied yet to the state)
std::vector<Steppable *> newAttackSteppable(const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p);

} // namespace octopus


#endif
