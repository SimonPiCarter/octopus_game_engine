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

void DotModifier::newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p, bool disableMainAttack_p) const
{
    Handle idx_l = state_p.getFlyingCommandHandle(step_p.getFlyingCommandSpawned());
    vec_r.push_back(new FlyingCommandSpawnStep(new DamageOverTime(idx_l, _tickRate, _nbOfTicks, _dmg, target_p._handle)));

    if(!disableMainAttack_p)
    {
        Fixed dmg_l = std::min(Fixed(-1), target_p.getArmor() - ent_p.getDamage(target_p._model));
        vec_r.push_back(new EntityHitPointChangeStep(target_p._handle, dmg_l));
    }
}

} // namespace octopus
