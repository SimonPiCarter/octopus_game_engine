#include "AoEModifier.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/Step.hh"

namespace octopus
{

std::vector<Steppable *> AoEModifier::newAttackSteppable(const Entity &ent_p, const Entity &target_p, State const &state_p, Step const &step_p) const
{
    std::vector<Steppable *> vec_l;

    Player const *player_l = state_p.getPlayer(ent_p._player);
    unsigned long team_l = player_l->_team;

    /// @brief get all within range
    TargetPanel panel_l(lookUpNewTargets(state_p, target_p._handle, _range, false));

    for(Entity const * subTarget_l : panel_l.units)
    {
        Player const *otherPlayer_l = state_p.getPlayer(subTarget_l->_player);
        if(otherPlayer_l->_team != team_l)
        {
            double curHp_l = subTarget_l->_hp + step_p.getHpChange(subTarget_l->_handle);
            double maxHp_l = subTarget_l->getHpMax();
            vec_l.push_back(new EntityHitPointChangeStep(subTarget_l->_handle, - _ratio * ent_p.getDamageNoBonus(), curHp_l, maxHp_l));
        }
    }
    for(Entity const * subTarget_l : panel_l.buildings)
    {
        Player const *otherPlayer_l = state_p.getPlayer(subTarget_l->_player);
        if(otherPlayer_l->_team != team_l)
        {
            double curHp_l = subTarget_l->_hp + step_p.getHpChange(subTarget_l->_handle);
            double maxHp_l = subTarget_l->getHpMax();
            vec_l.push_back(new EntityHitPointChangeStep(subTarget_l->_handle, - _ratio * ent_p.getDamageNoBonus(), curHp_l, maxHp_l));
        }
    }

	double curHp_l = target_p._hp + step_p.getHpChange(target_p._handle);
	double maxHp_l = target_p.getHpMax();
	double dmg_l = std::min(-1., target_p.getArmor() - ent_p.getDamage(target_p._model));
	vec_l.push_back(new EntityHitPointChangeStep(target_p._handle, dmg_l, curHp_l, maxHp_l));

    return vec_l;
}

} // namespace octopus
