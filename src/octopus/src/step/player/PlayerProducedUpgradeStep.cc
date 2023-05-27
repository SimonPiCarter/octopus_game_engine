#include "PlayerProducedUpgradeStep.hh"
#include "state/entity/Resource.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void PlayerProducedUpgradeStep::apply(State &state_p) const
{
    Logger::getDebug() << "PlayerProducedUpgradeStep :: apply " << _player <<std::endl;
    if(_add)
    {
        state_p.getPlayer(_player)->_producedUpgrade.insert(_upgrade);
    }
    else
    {
        state_p.getPlayer(_player)->_producedUpgrade.erase(_upgrade);
    }
}

void PlayerProducedUpgradeStep::revert(State &state_p, SteppableData const *data_p) const
{
    Logger::getDebug() << "PlayerProducedUpgradeStep :: revert " << _player <<std::endl;
    PlayerProducedUpgradeData const *typpedData_l = dynamic_cast<PlayerProducedUpgradeData const *>(data_p);
    /// only revert if need change has been applied
    if(typpedData_l->needChange)
    {
        if(_add)
        {
            state_p.getPlayer(_player)->_producedUpgrade.erase(_upgrade);
        }
        else
        {
            state_p.getPlayer(_player)->_producedUpgrade.insert(_upgrade);
        }
    }
}

SteppableData * PlayerProducedUpgradeStep::newData(State const &state_p) const
{
    PlayerProducedUpgradeData * data_l = new PlayerProducedUpgradeData();

    if(_add)
    {
        data_l->needChange =
            state_p.getPlayer(_player)->_producedUpgrade.find(_upgrade) == state_p.getPlayer(_player)->_producedUpgrade.end();
    }
    else
    {
        data_l->needChange =
            state_p.getPlayer(_player)->_producedUpgrade.find(_upgrade) != state_p.getPlayer(_player)->_producedUpgrade.end();
    }

    return data_l;
}

} // namespace octopus
