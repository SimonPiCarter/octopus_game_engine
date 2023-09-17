#include "AttackModifier.hh"

#include "AttackModifierData.hh"

#include "state/entity/Entity.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

void applyMainAttack(Step &step_p, AttackModifierData const &data_p, State const &state_p)
{
	if(state_p.isEntityAlive(data_p.target))
	{
		Entity const & target_l = *state_p.getLoseEntity(data_p.target.index);
		Fixed curHp_l = target_l._hp + step_p.getHpChange(data_p.target);
		Fixed maxHp_l = target_l.getHpMax();
		Fixed dmg_l = std::min(Fixed(-1), target_l.getArmor() - data_p.bonusDamage);
		step_p.addSteppable(new EntityHitPointChangeStep(target_l._handle, dmg_l, curHp_l, maxHp_l));
	}
}

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
