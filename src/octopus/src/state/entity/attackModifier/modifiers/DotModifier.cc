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

std::vector<Steppable *> DotModifier::newAttackSteppable(const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p) const
{
    std::vector<Steppable *> vec_l;

    Handle idx_l = state_p.getFlyingCommandHandle(step_p.getFlyingCommandSpawned());
    vec_l.push_back(new FlyingCommandSpawnStep(new DamageOverTime(idx_l, _tickRate, _nbOfTicks, _dmg, target_p._handle)));

	double dmg_l = std::min(-1., target_p.getArmor() - ent_p.getDamage(target_p._model));
    double curHp_l = target_p._hp + step_p.getHpChange(target_p._handle);
    double maxHp_l = target_p.getHpMax();
    vec_l.push_back(new EntityHitPointChangeStep(target_p._handle, dmg_l, curHp_l, maxHp_l));

    return vec_l;
}

} // namespace octopus
