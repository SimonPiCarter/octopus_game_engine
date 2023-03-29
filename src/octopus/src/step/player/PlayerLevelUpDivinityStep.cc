#include "PlayerLevelUpDivinityStep.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "logger/Logger.hh"

namespace octopus
{

void PlayerLevelUpDivinityStep::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerLevelUpDivinityStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
	player_l->_divLvl[_type] += 1;
}
void PlayerLevelUpDivinityStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "PlayerLevelUpDivinityStep :: revert " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
	player_l->_divLvl[_type] -= 1;
}

} // namespace octopus

