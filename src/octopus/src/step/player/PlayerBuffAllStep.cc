#include "PlayerBuffAllStep.hh"

#include "state/Handle.hh"
#include "state/State.hh"
#include "state/player/Player.hh"

#include "logger/Logger.hh"

namespace octopus
{

PlayerBuffAllStepData::~PlayerBuffAllStepData()
{
    for(SteppableData const *ptr_l : _buffStepsData)
    {
        delete ptr_l;
    }
}


void PlayerBuffAllStep::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerBuffAllStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);

    // Register buff in the player permanent buff
    player_l->_mapBuffs[_model].push_back(_buff);

    // Apply buff to all entities
    for(Entity *ent_l : state_p.getEntities())
    {
        // skip if wrong player or model
        if(ent_l->_player != _player
        || (ent_l->_model._id != _model && !_model.empty()))
        {
            continue;
        }
        EntityBuffStep buffStep_l(ent_l->_handle, _buff);
        buffStep_l.apply(state_p);
    }
}

void PlayerBuffAllStep::revert(State &state_p, SteppableData const *data_p) const
{
	Logger::getDebug() << "PlayerBuffAllStep :: revert " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
    PlayerBuffAllStepData const * data_l = dynamic_cast<PlayerBuffAllStepData const *>(data_p);

    if(player_l->_mapBuffs[_model].back()._id != _buff._id)
    {
        throw std::logic_error("PlayerBuffAllStep was revert in wrong order");
    }

    player_l->_mapBuffs[_model].pop_back();

    // Revert buff to all entities
    for(Entity *ent_l : state_p.getEntities())
    {
        // skip if wrong player or model
        if(ent_l->_player != _player
        || ent_l->_model._id != _model)
        {
            continue;
        }
        EntityBuffStep buffStep_l(ent_l->_handle, _buff);
        buffStep_l.revert(state_p, data_l->_buffStepsData[ent_l->_handle]);
    }
}

SteppableData * PlayerBuffAllStep::newData(State const &state_p) const
{
    PlayerBuffAllStepData * data_l = new PlayerBuffAllStepData();

    data_l->_buffStepsData.resize(state_p.getEntities().size(), nullptr);
    for(Entity const *ent_l : state_p.getEntities())
    {
        // skip if wrong player or model
        if(ent_l->_player != _player
        || ent_l->_model._id != _model)
        {
            continue;
        }
        EntityBuffStep buffStep_l(ent_l->_handle, _buff);

        data_l->_buffStepsData[ent_l->_handle] = buffStep_l.newData(state_p);
    }

    return data_l;
}

} // namespace octopus

