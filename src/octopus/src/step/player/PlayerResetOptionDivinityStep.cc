#include "PlayerResetOptionDivinityStep.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "logger/Logger.hh"

namespace octopus
{

void PlayerResetOptionDivinityStep::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerResetOptionDivinityStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
	player_l->_divOptions.pop_back();
}
void PlayerResetOptionDivinityStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "PlayerResetOptionDivinityStep :: revert " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
	player_l->_divOptions.push_back(_option);
}

} // namespace octopus

