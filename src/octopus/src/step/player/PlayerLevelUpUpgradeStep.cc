#include "PlayerLevelUpUpgradeStep.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "logger/Logger.hh"

namespace octopus
{

void PlayerLevelUpUpgradeStep::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerLevelUpUpgradeStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
	player_l->_upgradeLvl[_type] += 1;
}
void PlayerLevelUpUpgradeStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "PlayerLevelUpUpgradeStep :: revert " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
	player_l->_upgradeLvl[_type] -= 1;
}

} // namespace octopus

