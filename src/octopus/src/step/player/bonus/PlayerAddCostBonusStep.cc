#include "PlayerAddCostBonusStep.hh"

#include "state/State.hh"
#include "state/player/Player.hh"
#include "logger/Logger.hh"

namespace octopus
{

void PlayerAddCostBonusStep::apply(State &state_p) const
{
	Logger::getDebug() << "PlayerAddCostBonusStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);

    player_l->_bonuses._bonusCost[_model]._resourceModifier[_res] = _buff;
}

void PlayerAddCostBonusStep::revert(State &state_p, SteppableData const *data_p) const
{
	Logger::getDebug() << "PlayerAddCostBonusStep :: apply " << this->_player<<std::endl;
	Player *player_l = state_p.getPlayer(_player);
    PlayerCostBonusData const *data_l = static_cast<PlayerCostBonusData const *>(data_p);

    if(data_l)
    {
        player_l->_bonuses._bonusCost[_model]._resourceModifier[_res] = data_l->oldBuff;
    }
    else
    {
        player_l->_bonuses._bonusCost[_model]._resourceModifier.erase(_res);
    }
}

SteppableData * PlayerAddCostBonusStep::newData(State const &state_p) const
{
    Player const *player_l = state_p.getPlayer(_player);

	auto &&it_l = player_l->_bonuses._bonusCost.find(_model);
	if(it_l != player_l->_bonuses._bonusCost.end())
	{
	    auto &&itRes_l = it_l->second._resourceModifier.find(_res);
        if(itRes_l != it_l->second._resourceModifier.end())
        {
            return new PlayerCostBonusData(itRes_l->second);
        }
	}
    return nullptr;
}

} // octopus
