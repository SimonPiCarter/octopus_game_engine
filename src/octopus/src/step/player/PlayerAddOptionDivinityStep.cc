#include "PlayerAddOptionDivinityStep.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "logger/Logger.hh"

namespace octopus
{

void PlayerAddOptionDivinityStep::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerAddOptionDivinityStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
	player_l->_divOptions.push_back(DivinityOption {_lvlUp, _types});
}
void PlayerAddOptionDivinityStep::revert(State &state_p) const
{
	Logger::getDebug() << "PlayerAddOptionDivinityStep :: revert " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
	player_l->_divOptions.pop_back();
}

} // namespace octopus

