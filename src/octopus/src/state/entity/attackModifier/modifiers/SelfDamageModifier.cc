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

void SelfDamageModifier::newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &,
    State const &state_p, CommandContext const &, Step const &step_p, bool) const
{
	Fixed curHp_l = ent_p._hp + step_p.getHpChange(ent_p._handle);
	Fixed maxHp_l = ent_p.getHpMax();
    Fixed dmg_l = _damage + (ent_p._hp + step_p.getHpChange(ent_p._handle)) * _ratioCurHp + ent_p.getHpMax() * _ratioMaxHp;
    // if cannot kill we set hp to 1 at minimum
    if(!_canKill && dmg_l >= curHp_l)
    {
        dmg_l = curHp_l-1.;
    }
    vec_r.push_back(new EntityHitPointChangeStep(ent_p._handle, -dmg_l, curHp_l, maxHp_l));
}

} // namespace octopus
