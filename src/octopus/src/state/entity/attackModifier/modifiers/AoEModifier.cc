#include "AoEModifier.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

void AoEModifier::newAttackSteppable(StepShallow &step_p, AttackModifierData const &data_p, State const &state_p, bool disableMainAttack_p) const
{
    unsigned long team_l = data_p.team;

    /// @brief get all within range
    TargetPanel panel_l(lookUpNewTargets(state_p, data_p.target.index, data_p.targetPosition, data_p.team, _range, false));

    for(Entity const * subTarget_l : panel_l.units)
    {
        Player const *otherPlayer_l = state_p.getPlayer(subTarget_l->_player);
        if(otherPlayer_l->_team != team_l || _friendlyFire)
        {
            step_p.addSteppable(new EntityHitPointChangeStep(subTarget_l->_handle, - _ratio * data_p.baseDamage));
        }
    }
    for(Entity const * subTarget_l : panel_l.buildings)
    {
        Player const *otherPlayer_l = state_p.getPlayer(subTarget_l->_player);
        if(otherPlayer_l->_team != team_l || _friendlyFire)
        {
            step_p.addSteppable(new EntityHitPointChangeStep(subTarget_l->_handle, - _ratio * data_p.baseDamage));
        }
    }

    if(!disableMainAttack_p)
    {
		applyMainAttack(step_p, data_p, state_p);
    }
}

} // namespace octopus
