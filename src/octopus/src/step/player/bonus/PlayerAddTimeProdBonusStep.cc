#include "PlayerAddTimeProdBonusStep.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "logger/Logger.hh"

namespace octopus
{

void PlayerAddTimeProdBonusStep::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerAddTimeProdBonusStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);

    player_l->_bonuses._bonusTimeProduction[_model] = _buff;
}

void PlayerAddTimeProdBonusStep::revert(State &state_p, SteppableData const *data_p) const
{
	Logger::getDebug() << "PlayerAddTimeProdBonusStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
    PlayerTimeProdBonusData const *data_l = static_cast<PlayerTimeProdBonusData const *>(data_p);

    if(data_l)
    {
        player_l->_bonuses._bonusTimeProduction[_model] = data_l->oldBuff;
    }
    else
    {
        player_l->_bonuses._bonusTimeProduction.erase(_model);
    }
}

SteppableData * PlayerAddTimeProdBonusStep::newData(State const &state_p) const
{
    Player const *player_l = state_p.getPlayer(_player);

	auto &&it_l = player_l->_bonuses._bonusTimeProduction.find(_model);
	if(it_l == player_l->_bonuses._bonusTimeProduction.end())
	{
		return nullptr;
	}
    return new PlayerTimeProdBonusData(it_l->second);
}

} // octopus
