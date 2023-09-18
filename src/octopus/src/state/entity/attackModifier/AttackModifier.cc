#include "AttackModifier.hh"

#include "AttackModifierData.hh"

#include "state/entity/Entity.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

void NoModifier::newAttackSteppable(Step &step_p, AttackModifierData const &data_p, State const &state_p, bool) const
{
	applyMainAttack(step_p, data_p, state_p);
}

void CompositeModifier::newAttackSteppable(Step &step_p, AttackModifierData const &data_p, State const &state_p) const
{
	for(CompositableModifier const &mod_l : _modifiers)
	{
		// visit modifier
		std::visit([&](auto&& arg)
		{
			arg.newAttackSteppable(step_p, data_p, state_p, true);
		}, mod_l);
	}
}

void newAttackSteppable(AttackModifier const &attackMod_p, Step &step_p, AttackModifierData const &data_p, State const &state_p)
{
	// visit modifier
	std::visit([&](auto&& arg)
	{
		arg.newAttackSteppable(step_p, data_p, state_p);
	}, attackMod_p);
}

} // namespace octopus
