#include "LifeStealModifier.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

void LifeStealModifier::newAttackSteppable(Step &step_p, AttackModifierData const &data_p, State const &state_p, bool disableMainAttack_p) const
{
    if(!disableMainAttack_p)
    {
		applyMainAttack(step_p, data_p, state_p);
    }
    // lifesteal
	if(state_p.isEntityAlive(data_p.source))
	{
		Entity const & source_l = *state_p.getLoseEntity(data_p.source.index);
		// damage
        Fixed curHp_l = source_l._hp + step_p.getHpChange(data_p.source);
        Fixed maxHp_l = source_l.getHpMax();
	    step_p.addSteppable(new EntityHitPointChangeStep(data_p.source, -data_p.baseDamage*_ratio, curHp_l, maxHp_l));
    }
}

} // namespace octopus
