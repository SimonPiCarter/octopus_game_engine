#include "DamageOverTime.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

void DamageOverTime::applyEffect(Step & step_p, State const &state_p, CommandData const *, PathManager &) const
{
    step_p.addSteppable(new EntityHitPointChangeStep(_ent, -_dmg));
}

}
