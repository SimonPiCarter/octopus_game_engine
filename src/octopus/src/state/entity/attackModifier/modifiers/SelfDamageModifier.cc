#include "SelfDamageModifier.hh"

#include "command/flying/CommandEffectOverTime.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "step/command/flying/FlyingCommandSpawnStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

void SelfDamageModifier::newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &, State const &state_p, Step const &step_p, bool) const
{
    Fixed dmg_l = _damage + ent_p._hp * _ratioCurHp + ent_p.getHpMax() * _ratioMaxHp;
    vec_r.push_back(new EntityHitPointChangeStep(ent_p._handle, -dmg_l, !_canKill));
}

} // namespace octopus
