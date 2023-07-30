#include "PlayerSpawnStep.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "logger/Logger.hh"

namespace octopus
{

void PlayerSpawnStep::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerSpawnStep :: apply id = " << this->_playerIdx << " team = " <<this->_playerTeam<<std::endl;

	if(state_p.getPlayers().size() != _playerIdx)
	{
		throw std::logic_error("Cannot create player with discontinious id and in the wrong order");
	}
	Player * player_l = new Player();
	player_l->_id = _playerIdx;
	player_l->_team = _playerTeam;
	state_p.getPlayers().push_back(player_l);
}

void PlayerSpawnStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "PlayerSpawnStep :: revert id = " << this->_playerIdx << " team = " <<this->_playerTeam<<std::endl;

	Player * player_l = state_p.getPlayers().back();
	state_p.getPlayers().pop_back();
	if(player_l->_id != _playerIdx)
	{
		throw std::logic_error("Cannot create player with discontinious id and in the wrong order");
	}
	delete player_l;
}


} // namespace octopus

