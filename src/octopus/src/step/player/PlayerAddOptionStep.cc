#include "PlayerAddOptionStep.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "logger/Logger.hh"

namespace octopus
{

void PlayerAddOptionStep::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerAddOptionStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
    if(player_l->_options[_key] != nullptr)
    {
        throw std::logic_error("Error while applying option : an option with the given key already exists "+_key);
    }
    player_l->_options[_key] = _generator->newCopy();
}

void PlayerAddOptionStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "PlayerAddOptionStep :: revert " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
    if(player_l->_options[_key] == nullptr)
    {
        throw std::logic_error("Error while reverting option : an option with the given key already exists "+_key);
    }
    delete player_l->_options[_key];
    player_l->_options[_key] = nullptr;
}

} // namespace octopus

