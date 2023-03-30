#include "AttackModifier.hh"

#include "state/entity/Entity.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

std::vector<Steppable *> NoModifier::newAttackSteppable(const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p) const
{
	double curHp_l = target_p._hp + step_p.getHpChange(target_p._handle);
	double maxHp_l = target_p.getHpMax();
	double dmg_l = std::min(-1., target_p.getArmor() - ent_p.getDamage(target_p._model));
	return {new EntityHitPointChangeStep(target_p._handle, dmg_l, curHp_l, maxHp_l)};
}

std::vector<Steppable *> newAttackSteppable(const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p)
{
    std::vector<Steppable *> vec_l;

	// visit modifier
	std::visit([&](auto&& arg)
	{
		vec_l = arg.newAttackSteppable(ent_p, target_p, state_p, step_p);
	}, ent_p._attackMod);

	return vec_l;
}

} // namespace octopus
