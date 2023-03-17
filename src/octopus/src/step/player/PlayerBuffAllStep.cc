#include "PlayerBuffAllStep.hh"

#include "state/Handle.hh"
#include "state/State.hh"
#include "state/player/Player.hh"

#include "logger/Logger.hh"

namespace octopus
{

void PlayerBuffAllStep::apply(State &state_p, SteppableData *data_p) const
{
	Logger::getDebug() << "PlayerBuffAllStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
    PlayerBuffAllStepData * data_l = dynamic_cast<PlayerBuffAllStepData*>(data_p);

    // Register buff in the player permanent buff
    player_l->_mapBuffs[_model].push_back(_buff);

    if(data_l->_buffSteps.empty())
    {
        // Apply buff to all entities
        for(Entity *ent_l : state_p.getEntities())
        {
            // skip if wrong player or model
            if(ent_l->_player != _player
            || ent_l->_model._id != _model)
            {
                continue;
            }
            // create step
            auto &&it_l = ent_l->_timeSinceBuff.find(_buff._id);
            bool new_l = it_l == ent_l->_timeSinceBuff.cend();
            unsigned long time_l = 0;
            if(!new_l)
            {
                time_l = it_l->second;
            }
            data_l->_buffSteps.emplace_back(ent_l->_handle, _buff, time_l, new_l);
        }
    }

    for(EntityBuffStep const &step_l : data_l->_buffSteps)
    {
        // no data required
        step_l.apply(state_p, nullptr);
    }
}
void PlayerBuffAllStep::revert(State &state_p, SteppableData *data_p) const
{
	Logger::getDebug() << "PlayerBuffAllStep :: revert " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
    PlayerBuffAllStepData * data_l = dynamic_cast<PlayerBuffAllStepData*>(data_p);

    if(player_l->_mapBuffs[_model].back()._id != _buff._id)
    {
        throw std::logic_error("PlayerBuffAllStep was revert in wrong order");
    }
    player_l->_mapBuffs[_model].pop_back();

    for(EntityBuffStep const &step_l : data_l->_buffSteps)
    {
        // no data required
        step_l.revert(state_p, nullptr);
    }
}

} // namespace octopus

