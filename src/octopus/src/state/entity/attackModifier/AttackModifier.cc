#include "AttackModifier.hh"

#include "state/entity/Entity.hh"
#include "step/entity/EntityHitPointChangeStep.hh"

namespace octopus
{

AttackModifier::~AttackModifier() {}

Steppable * newAttackSteppable(const Entity &ent_p, const Entity &target_p, State const &state_p)
{
	if(ent_p._attackMod)
	{
		return ent_p._attackMod->newAttackSteppable(ent_p, target_p, state_p);
	}
	return new EntityHitPointChangeStep(target_p._handle, std::min(-1., target_p.getArmor() - ent_p.getDamage(target_p._model)));
}

} // namespace octopus
