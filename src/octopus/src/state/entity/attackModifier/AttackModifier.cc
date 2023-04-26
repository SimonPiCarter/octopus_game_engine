#include "AttackModifier.hh"

#include "state/entity/Entity.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

void NoModifier::newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p, bool) const
{
	double curHp_l = target_p._hp + step_p.getHpChange(target_p._handle);
	double maxHp_l = target_p.getHpMax();
	double dmg_l = std::min(-1., target_p.getArmor() - ent_p.getDamage(target_p._model));
	vec_r.push_back(new EntityHitPointChangeStep(target_p._handle, dmg_l, curHp_l, maxHp_l));
}

void CompositeModifier::newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p) const
{
	for(CompositableModifier const &mod_l : _modifiers)
	{
		// visit modifier
		std::visit([&](auto&& arg)
		{
			arg.newAttackSteppable(vec_r, ent_p, target_p, state_p, step_p, true);
		}, mod_l);
	}
}

void newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p)
{
	// visit modifier
	std::visit([&](auto&& arg)
	{
		arg.newAttackSteppable(vec_r, ent_p, target_p, state_p, step_p);
	}, ent_p._attackMod);
}

} // namespace octopus
