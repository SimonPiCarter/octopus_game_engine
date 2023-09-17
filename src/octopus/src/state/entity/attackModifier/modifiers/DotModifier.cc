#include "DotModifier.hh"

#include "command/flying/eot/DamageOverTime.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "step/command/flying/FlyingCommandSpawnStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

void DotModifier::newAttackSteppable(Step &step_p, AttackModifierData const &data_p, State const &state_p, bool disableMainAttack_p) const
{
    Handle idx_l = state_p.getFlyingCommandHandle(step_p.getFlyingCommandSpawned());
    step_p.addSteppable(new FlyingCommandSpawnStep(new DamageOverTime(idx_l, _tickRate, _nbOfTicks, _dmg, data_p.target)));

    if(!disableMainAttack_p)
    {
		applyMainAttack(step_p, data_p, state_p);
    }
}

} // namespace octopus
