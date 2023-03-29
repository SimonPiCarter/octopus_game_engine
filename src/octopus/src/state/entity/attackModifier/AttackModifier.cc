#include "AttackModifier.hh"

#include "state/entity/Entity.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

AttackModifier::~AttackModifier() {}

Steppable * newAttackSteppable(const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p)
{
	double curHp_l = target_p._hp + step_p.getHpChange(target_p._handle);
	double maxHp_l = target_p.getHpMax();
	if(ent_p._attackMod)
	{
		return ent_p._attackMod->newAttackSteppable(ent_p, target_p, state_p, curHp_l, maxHp_l);
	}
	double dmg_l = std::min(-1., target_p.getArmor() - ent_p.getDamage(target_p._model));
	return new EntityHitPointChangeStep(target_p._handle, dmg_l, curHp_l, maxHp_l);
}

} // namespace octopus
