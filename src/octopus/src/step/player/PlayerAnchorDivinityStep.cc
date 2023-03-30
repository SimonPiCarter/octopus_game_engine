#include "PlayerAnchorDivinityStep.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "logger/Logger.hh"

namespace octopus
{

void PlayerAnchorDivinityStep::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerAnchorDivinityStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
	player_l->_divAnchor[_type] = _new;
}
void PlayerAnchorDivinityStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "PlayerAnchorDivinityStep :: revert " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
	player_l->_divAnchor[_type] = _old;
}

} // namespace octopus

