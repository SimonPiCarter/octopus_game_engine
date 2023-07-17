#ifndef __AttackModifier__
#define __AttackModifier__

#include <vector>
#include <variant>

#include "modifiers/AoEModifier.hh"
#include "modifiers/ChainingModifier.hh"
#include "modifiers/DotModifier.hh"
#include "modifiers/LifeStealModifier.hh"
#include "modifiers/SelfDamageModifier.hh"

namespace octopus
{
class Entity;
class State;
class Step;
class Steppable;
struct CommandContext;

/// @brief this class is aimed at dealing aoe damage
class NoModifier
{
public:
	/// @brief create a new attack steppable
	void newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &target_p,
		State const &state_p, CommandContext const &commandContext_p, Step const &step_p, bool disableMainAttack_p=false) const;
};

/// @brief CompositeModifier allow to stack modifiers
/// @warning the basic attack will be disabled on every modifier
/// only NoModifier will still apply it, it you want basic damage to be applied you need to include NoModifier in the modifiers
class CompositeModifier
{
using CompositableModifier = std::variant<
	NoModifier,
	AoEModifier,
	ChainingModifier,
	DotModifier,
	LifeStealModifier,
	SelfDamageModifier
>;
public:
	std::vector<CompositableModifier> _modifiers;

	/// @brief create a new attack steppable based on components
	void newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &target_p,
		State const &state_p, CommandContext const &commandContext_p, Step const &step_p) const;
};

using AttackModifier = std::variant<
	NoModifier,
	CompositeModifier,
	AoEModifier,
	ChainingModifier,
	DotModifier,
	LifeStealModifier,
	SelfDamageModifier
>;

/// @brief create attack steppable using eventual attack modifier
/// @param currentTargetHp_p the hp of the target with deduction of every hp change in the current step (not applied yet to the state)
void newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &target_p,
	State const &state_p, CommandContext const &commandContext_p, Step const &step_p);

} // namespace octopus


#endif
