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

void PlayerProducedUpgradeStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "PlayerProducedUpgradeStep :: revert " << _player <<std::endl;
	if(_add)
    {
        state_p.getPlayer(_player)->_producedUpgrade.erase(_upgrade);
    }
    else
    {
        state_p.getPlayer(_player)->_producedUpgrade.insert(_upgrade);
    }
}

} // namespace octopus
