#include "AoEModifier.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

void AoEModifier::newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p, bool disableMainAttack_p) const
{
    Player const *player_l = state_p.getPlayer(ent_p._player);
    unsigned long team_l = player_l->_team;

    /// @brief get all within range
    TargetPanel panel_l(lookUpNewTargets(state_p, target_p._handle, _range, false));

    for(Entity const * subTarget_l : panel_l.units)
    {
        Player const *otherPlayer_l = state_p.getPlayer(subTarget_l->_player);
        if(otherPlayer_l->_team != team_l || _friendlyFire)
        {
            vec_r.push_back(new EntityHitPointChangeStep(subTarget_l->_handle, - _ratio * ent_p.getDamageNoBonus()));
        }
    }
    for(Entity const * subTarget_l : panel_l.buildings)
    {
        Player const *otherPlayer_l = state_p.getPlayer(subTarget_l->_player);
        if(otherPlayer_l->_team != team_l || _friendlyFire)
        {
            vec_r.push_back(new EntityHitPointChangeStep(subTarget_l->_handle, - _ratio * ent_p.getDamageNoBonus()));
        }
    }

    if(!disableMainAttack_p)
    {
        Fixed dmg_l = std::min(Fixed(-1), target_p.getArmor() - ent_p.getDamage(target_p._model));
        vec_r.push_back(new EntityHitPointChangeStep(target_p._handle, dmg_l));
    }
}

} // namespace octopus
