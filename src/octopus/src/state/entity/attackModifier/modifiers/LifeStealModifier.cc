#include "LifeStealModifier.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

void LifeStealModifier::newAttackSteppable(StepShallow &step_p, AttackModifierData const &data_p, State const &state_p, bool disableMainAttack_p) const
{
    if(!disableMainAttack_p)
    {
		applyMainAttack(step_p, data_p, state_p);
    }
    // lifesteal
	if(state_p.isEntityAlive(data_p.source))
	{
	    step_p.addSteppable(new EntityHitPointChangeStep(data_p.source, data_p.baseDamage*_ratio));
  }
}

} // namespace octopus
