#include "PlayerAttackModAllStep.hh"

#include "state/Handle.hh"
#include "state/State.hh"
#include "state/player/Player.hh"

#include "logger/Logger.hh"

namespace octopus
{

PlayerAttackModAllStepData::~PlayerAttackModAllStepData()
{
    for(SteppableData const *ptr_l : _stepsData)
    {
        delete ptr_l;
    }
}


void PlayerAttackModAllStep::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerAttackModAllStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);

    player_l->_mapModifiers[_model] = _mod;

    // Apply buff to all entities
    for(Entity *ent_l : state_p.getEntities())
    {
        // skip if wrong player or model
        if(ent_l->_player != _player
        || ent_l->_model._id != _model)
        {
            continue;
        }
        AttackModifierStep steppable_l(ent_l->_handle, _mod);
        steppable_l.apply(state_p);
    }
}

void PlayerAttackModAllStep::revert(State &state_p, SteppableData const *data_p) const
{
	Logger::getDebug() << "PlayerAttackModAllStep :: revert " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
    PlayerAttackModAllStepData const * data_l = dynamic_cast<PlayerAttackModAllStepData const *>(data_p);

    player_l->_mapModifiers[_model] = data_l->_old;

    // Revert buff to all entities
    for(Entity *ent_l : state_p.getEntities())
    {
        // skip if wrong player or model
        if(ent_l->_player != _player
        || ent_l->_model._id != _model)
        {
            continue;
        }
        AttackModifierStep steppable_l(ent_l->_handle, _mod);
        steppable_l.revert(state_p, data_l->_stepsData[ent_l->_handle]);
    }
}

SteppableData * PlayerAttackModAllStep::newData(State const &state_p) const
{
    PlayerAttackModAllStepData * data_l = new PlayerAttackModAllStepData();

	Player const *player_l = state_p.getPlayer(_player);
    auto &&it_l = player_l->_mapModifiers.find(_model);
    if(it_l != player_l->_mapModifiers.end())
    {
        data_l->_old = it_l->second;
    }

    data_l->_stepsData.resize(state_p.getEntities().size(), nullptr);
    for(Entity const *ent_l : state_p.getEntities())
    {
        // skip if wrong player or model
        if(ent_l->_player != _player
        || ent_l->_model._id != _model)
        {
            continue;
        }
        AttackModifierStep steppable_l(ent_l->_handle, _mod);

        data_l->_stepsData[ent_l->_handle] = steppable_l.newData(state_p);
    }

    return data_l;
}

} // namespace octopus

