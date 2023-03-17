#include "TimerDamage.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/Step.hh"

void TimerDamage::applyEffect(octopus::Step & step_p, octopus::State const &state_p, octopus::CommandData const *, octopus::PathManager &) const
{
    const octopus::Player *player_l = state_p.getPlayer(_player);
    double res_l = octopus::getResource(*player_l, _res);
    // If resource > 0 decrease it
    if(res_l > 1e-3)
    {
        std::map<octopus::ResourceType, double> map_l;
        map_l[_res] = 1;
        step_p.addSteppable(new octopus::PlayerSpendResourceStep(_player, map_l));
    }
    else
    {
        step_p.addSteppable(new octopus::EntityHitPointChangeStep(_ent, -10.));
    }
}