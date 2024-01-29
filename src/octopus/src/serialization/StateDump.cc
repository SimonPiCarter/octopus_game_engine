#include "StateDump.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"

#include "crc/FastCRC.hh"

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

uint32_t setup(int64_t const &data_p, FastCRC32 &encoder_p)
{
    return encoder_p.crc32((const uint8_t *)&data_p, sizeof(data_p));
}
uint32_t encode(int64_t const &data_p, FastCRC32 &encoder_p)
{
    return encoder_p.crc32_upd((const uint8_t *)&data_p, sizeof(data_p));
}

uint32_t hashState(State const &state_p)
{
    uint32_t checksum_l = 0;

    FastCRC32 encoder_l;

    checksum_l = setup(state_p.getStepApplied(), encoder_l);
    for(Player const *player_l : state_p.getPlayers())
    {
        checksum_l = encode(player_l->_id, encoder_l);
        for(auto &&pair_l : player_l->_resources)
        {
            checksum_l = encode(pair_l.second.data(), encoder_l);
        }
        for(auto &&pair_l : player_l->_upgradeLvl)
        {
            checksum_l = encode(pair_l.second, encoder_l);
        }
    }
    for(Entity const * ent_l : state_p.getEntities())
    {
        checksum_l = encode(ent_l->_handle.revision, encoder_l);
        checksum_l = encode((ent_l->_alive?1:0), encoder_l);
        checksum_l = encode(ent_l->_hp.data(), encoder_l);
        checksum_l = encode(ent_l->_pos.x.data(), encoder_l);
        checksum_l = encode(ent_l->_pos.y.data(), encoder_l);
    }

    return checksum_l;
}

} // namespace octopus

