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
    player_l->_options.push_back(_generator->newCopy());
	// gen options if only option
	if(player_l->_options.size() == 1)
	{
		player_l->_options.front()->genOptions(state_p);
	}
}

void PlayerAddOptionStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "PlayerAddOptionStep :: revert " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
	if(player_l->_options.back()->_key != _key)
	{
        throw std::logic_error("Error while reverting option : "+_key);
    }
    delete player_l->_options.back();
    player_l->_options.pop_back();
}

} // namespace octopus

