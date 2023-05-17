#include "DamageOverTime.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

void DamageOverTime::applyEffect(Step & step_p, State const &state_p, CommandData const *, PathManager &) const
{
    double maxHp_l = state_p.getEntity(_ent)->getHpMax();
    double curHp_l = state_p.getEntity(_ent)->_hp + step_p.getHpChange(_ent);
    step_p.addSteppable(new EntityHitPointChangeStep(_ent, -_dmg, curHp_l, maxHp_l));
}

}
