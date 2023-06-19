#include "PlayerConditionalBuffAllStep.hh"

#include "state/Handle.hh"
#include "state/State.hh"
#include "state/player/Player.hh"

#include "logger/Logger.hh"

namespace octopus
{

void PlayerConditionalBuffAllStep::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerConditionalBuffAllStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);

    // Register buff in the player permanent buff
    player_l->_mapConditionalBuffs[_model].push_back(_buff);

    // Apply buff to all entities
    for(Entity *ent_l : state_p.getEntities())
    {
        // skip if wrong player or model
        if(ent_l->_player != _player
        || ent_l->_model._id != _model)
        {
            continue;
        }
        EntityConditionalBuffStep buffStep_l(ent_l->_handle, _buff);
        buffStep_l.apply(state_p);
    }
}

void PlayerConditionalBuffAllStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "PlayerConditionalBuffAllStep :: revert " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);

    if(player_l->_mapConditionalBuffs[_model].back()._id != _buff._id)
    {
        throw std::logic_error("PlayerConditionalBuffAllStep was revert in wrong order");
    }

    player_l->_mapConditionalBuffs[_model].pop_back();

    // Revert buff to all entities
    for(Entity *ent_l : state_p.getEntities())
    {
        // skip if wrong player or model
        if(ent_l->_player != _player
        || ent_l->_model._id != _model)
        {
            continue;
        }
        EntityConditionalBuffStep buffStep_l(ent_l->_handle, _buff);
        buffStep_l.revert(state_p, nullptr);
    }
}

} // namespace octopus

