#include "TimerDamage.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/Step.hh"

void TimerDamage::applyEffect(octopus::Step & step_p, octopus::State const &state_p, octopus::CommandData const *, octopus::CommandContext &) const
{
    const octopus::Player *player_l = state_p.getPlayer(_player);
    octopus::Fixed res_l = octopus::getResource(*player_l, _res);
    // If resource > 0 decrease it
    if(!is_zero(res_l))
    {
        std::map<std::string, octopus::Fixed> map_l;
        map_l[_res] = 1;
        step_p.addSteppable(new octopus::PlayerSpendResourceStep(_player, map_l));
    }
    else
    {
        octopus::Fixed maxHp_l = state_p.getEntity(_ent)->getHpMax();
        octopus::Fixed curHp_l = state_p.getEntity(_ent)->_hp + step_p.getHpChange(_ent);
        step_p.addSteppable(new octopus::EntityHitPointChangeStep(_ent, -10., curHp_l, maxHp_l));
    }
}
