#include "StateDump.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"

namespace octopus
{
void streamStateAsSimpleText(std::ostream &os_p, State const &state_p)
{
    os_p<<"Step "<<state_p.getStepApplied()<<std::endl;
    for(Player const *player_l : state_p.getPlayers())
    {
        os_p<<"Player "<<player_l->_id<<std::endl;
        os_p<<"Resources"<<std::endl;
        for(auto &&pair_l : player_l->_resources)
        {
            os_p<<"\t"<<pair_l.first<<" : "<<pair_l.second.data()<<std::endl;
        }
        os_p<<"Upgrades"<<std::endl;
        for(auto &&pair_l : player_l->_upgradeLvl)
        {
            os_p<<"\t"<<pair_l.first<<" : "<<pair_l.second<<std::endl;
        }
    }
    for(Entity const * ent_l : state_p.getEntities())
    {
        os_p<<"Entity "<<ent_l->_handle<<std::endl;
        os_p<<"\talive "<<(ent_l->_alive?"1":"0")<<std::endl;
        os_p<<"\thp "<<ent_l->_hp<<std::endl;
        os_p<<"\tpos "<<ent_l->_pos.x.data()<<","<<ent_l->_pos.y.data()<<std::endl;
    }
}

} // namespace octopus

