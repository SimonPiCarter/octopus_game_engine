#include "AoEModifier.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"
#include "utils/KdTree.hh"

namespace octopus
{

void AoEModifier::newAttackSteppable(std::vector<Steppable *> &vec_r, const Entity &ent_p, const Entity &target_p,
    State const &state_p, CommandContext const &context_p, Step const &step_p, bool disableMainAttack_p) const
{
    Player const *player_l = state_p.getPlayer(ent_p._player);
    unsigned long team_l = player_l->_team;

    /// @brief get all within range
    std::vector<std::pair<Fixed, Entity const *> > neighbors_l = context_p.kdTree->computeEntityNeighbors(target_p._handle, _range, 0);

    for(auto && pair_l : neighbors_l)
    {
        Entity const * subTarget_l = pair_l.second;
        Player const *otherPlayer_l = state_p.getPlayer(subTarget_l->_player);
        if(otherPlayer_l->_team != team_l || _friendlyFire)
        {
            Fixed curHp_l = subTarget_l->_hp + step_p.getHpChange(subTarget_l->_handle);
            Fixed maxHp_l = subTarget_l->getHpMax();
            vec_r.push_back(new EntityHitPointChangeStep(subTarget_l->_handle, - _ratio * ent_p.getDamageNoBonus(), curHp_l, maxHp_l));
        }
    }

    if(!disableMainAttack_p)
    {
        Fixed curHp_l = target_p._hp + step_p.getHpChange(target_p._handle);
        Fixed maxHp_l = target_p.getHpMax();
        Fixed dmg_l = std::min(Fixed(-1), target_p.getArmor() - ent_p.getDamage(target_p._model));
        vec_r.push_back(new EntityHitPointChangeStep(target_p._handle, dmg_l, curHp_l, maxHp_l));
    }
}

} // namespace octopus
